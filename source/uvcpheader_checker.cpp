

#include <chrono>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <cstddef>
#include <stddef.h>

#include "utils/verbose.hpp"
#include "validuvc/uvcpheader_checker.hpp"
#include "validuvc/control_config.hpp"

#ifdef _WIN32
  typedef unsigned char u_char;
#endif

uint8_t UVCPHeaderChecker::payload_valid_ctrl(
    const std::vector<u_char>& uvc_payload,
    std::chrono::time_point<std::chrono::steady_clock> received_time) {
  // Make graph file
  // Make picture file having mjpeg, yuyv, h264

  static std::chrono::time_point<std::chrono::steady_clock> temp_received_time;

  if (uvc_payload.empty()) {
    v_cerr_5 << "UVC payload is empty." << std::endl;
    update_payload_error_stat(ERR_EMPTY_PAYLOAD);
    return ERR_EMPTY_PAYLOAD;
  }
  if (uvc_payload.size() > ControlConfig::dwMaxPayloadTransferSize) {
    v_cerr_5 << "UVC payload size exceeds maximum transfer size." << std::endl;
    update_payload_error_stat(ERR_MAX_PAYLAOD_OVERFLOW);
    return ERR_MAX_PAYLAOD_OVERFLOW;
  }

  if (temp_received_time == std::chrono::time_point<std::chrono::steady_clock>() ||
      std::chrono::duration_cast<std::chrono::seconds>(received_time - temp_received_time).count() >= 1) {
    
    std::cout << "FPS: " << frame_count.load() << " frames per second"
              << std::endl;

    int fps_difference = ControlConfig::fps - frame_count.load();
    if (frame_count != ControlConfig::fps){
      frame_stats.count_frame_drop += std::abs(fps_difference);
    }
    frame_count = 0;
    temp_received_time = received_time;

  }

  static UVC_Payload_Header previous_previous_payload_header;
  static uint8_t previous_previous_error = 0;
  static UVC_Payload_Header previous_payload_header;
  static uint8_t previous_error = 0;
  // static std::queue<std::vector<u_char>> fid_queue;

  UVC_Payload_Header payload_header =
      parse_uvc_payload_header(uvc_payload, received_time);

  UVCError payload_header_valid_return =
      payload_header_valid(payload_header, previous_payload_header,
                           previous_previous_payload_header);

  if (!payload_header_valid_return || payload_header_valid_return == ERR_MISSING_EOF) {

    bool frame_found = false;

    for (auto& frame : frames) {
      if (payload_header.PTS && frame->frame_pts == payload_header.PTS) {
        frame_found = true;

        frame->add_payload(payload_header, uvc_payload.size());
        frame->add_received_chrono_time(received_time);

        size_t total_payload_size = std::accumulate(frame->payload_sizes.begin(), frame->payload_sizes.end(), size_t(0));
        if (total_payload_size > ControlConfig::dwMaxVideoFrameSize) {
          frame->frame_error = ERR_FRAME_MAX_FRAME_OVERFLOW;  
        }

        if (payload_header_valid_return && payload_header_valid_return != ERR_MISSING_EOF) {
          frame->set_frame_error();  // if error set, add error flag to the frame
        }

        break;
      }
    }
    
    // create new frame if not found
    if (!frame_found || previous_payload_header.bmBFH.BFH_EOF) {

      //Process the last frame when EOF is missing
      if (payload_header_valid_return == ERR_MISSING_EOF) {
        v_cerr_2 << "Missing EOF" << std::endl;
        if (!frames.empty()) {
          auto& last_frame = frames.back();
          last_frame->frame_error = ERR_FRAME_ERROR;
          last_frame->eof_reached = false;
          //finish the last frame
          update_frame_error_stat(last_frame->frame_error);
          save_frames_to_log(last_frame);
          if (last_frame->frame_error) {
            v_cout_1 << "Frame Error_: " << last_frame->frame_error << std::endl;
            v_cout_1 << "Frame Payload times: ";
            for (const auto& time_point : last_frame->received_chrono_times) {
                v_cout_1 << std::chrono::duration_cast<std::chrono::nanoseconds>(time_point.time_since_epoch()).count() << "ns, ";
            }
            v_cout_1 << std::endl;
          }
          processed_frames.push_back(std::move(frames.back()));
          frames.pop_back();
          frame_count++;

          if (processed_frames.size() > 30) {
            processed_frames.erase(processed_frames.begin());
          }

        }
      }

      ++current_frame_number;
      frames.push_back(std::make_unique<ValidFrame>(current_frame_number));
      auto& new_frame = frames.back();
      new_frame->frame_pts = payload_header.PTS;  // frame pts == payload pts

      new_frame->add_payload(payload_header, uvc_payload.size());
      new_frame->add_received_chrono_time(received_time);

      size_t total_payload_size = std::accumulate(new_frame->payload_sizes.begin(), new_frame->payload_sizes.end(), size_t(0));
      if (total_payload_size > ControlConfig::dwMaxVideoFrameSize) {
        new_frame->frame_error = ERR_FRAME_MAX_FRAME_OVERFLOW;  
      }

      if (payload_header_valid_return && payload_header_valid_return != ERR_MISSING_EOF) {
        new_frame->set_frame_error();
      }
    }

    previous_payload_header = payload_header;
    previous_previous_payload_header = previous_payload_header;
  

    if (payload_header.bmBFH.BFH_EOF) {
      auto& last_frame = frames.back();
      last_frame->eof_reached = true;
      update_frame_error_stat(last_frame->frame_error);
      // finish the frame
      save_frames_to_log(frames.back());
      if (last_frame->frame_error) {
        v_cout_1 << "Frame Error__: " << last_frame->frame_error << std::endl;
        v_cout_1 << "Frame Payload times: ";
        for (const auto& time_point : last_frame->received_chrono_times) {
            v_cout_1 << std::chrono::duration_cast<std::chrono::nanoseconds>(time_point.time_since_epoch()).count() << "ns, ";
        }
        v_cout_1 << std::endl;
      }
      processed_frames.push_back(std::move(frames.back()));
      frames.pop_back();
      frame_count++;

      if (processed_frames.size() > 90) {
        processed_frames.erase(processed_frames.begin());
      }


      // Check the Frame width x height in here
      // For YUYV format, the width x height should be 1280 x 720 x 2 excluding
      // the headerlength If not then there is a problem with the frame
      if (ControlConfig::frame_format == "yuyv") {
        // Calculate the expected size for the YUYV frame
        size_t expected_frame_size =
            ControlConfig::get_width() * ControlConfig::get_height() * 2;

        // Calculate the actual size by summing up all payload sizes and
        // subtracting the total header lengths
        size_t actual_frame_size = 0;
        for (const auto& frame : frames) {
          for (size_t i = 0; i < frame->payload_sizes.size(); ++i) {
            actual_frame_size +=
                frame->payload_sizes[i] - frame->payload_headers[i].HLE;
          }
        }

        if (actual_frame_size != expected_frame_size) {
          v_cerr_2 << "Frame size mismatch for YUYV: expected "
                  << expected_frame_size << " but got " << actual_frame_size
                  << std::endl;
        }
      }
    }

    update_payload_error_stat(payload_header_valid_return);
    return payload_header_valid_return;

  } else {
    // TODO save in the error frame heap
    update_payload_error_stat(payload_header_valid_return);
    return payload_header_valid_return;
  }

  // v_cout_2 << "Payload is valid." << std::endl;
  update_payload_error_stat(ERR_UNKNOWN);
  return ERR_UNKNOWN;
}

void UVCPHeaderChecker::timer_thread() {
  while (!stop_timer_thread) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // std::cout << "FPS: " << frame_count.load() << " frames per second"
    //           << std::endl;

    // int fps_difference = ControlConfig::fps - frame_count.load();
    // if (frame_count != ControlConfig::fps){
    //   frame_stats.count_frame_drop += std::abs(fps_difference);
    // }
    // frame_count = 0;
  }
}


UVC_Payload_Header UVCPHeaderChecker::parse_uvc_payload_header(
    const std::vector<u_char>& uvc_payload,
    std::chrono::time_point<std::chrono::steady_clock> received_time) {
  UVC_Payload_Header payload_header = {};
  if (uvc_payload.size() < 2) {
    v_cerr_2 << "Error: UVC payload size is too small." << std::endl;
    //save_payload_header_to_log(payload_header, received_time);
    return payload_header;  // check if payload is too small for payload header
  }

  // copy hle, bfh
  payload_header.HLE = uvc_payload[0];
  payload_header.BFH = uvc_payload[1];

  size_t current_offset = 2;

  if (payload_header.bmBFH.BFH_PTS &&
      current_offset + sizeof(uint32_t) <= uvc_payload.size()) {
    // pts if present
    std::memcpy(&payload_header.PTS, &uvc_payload[current_offset],
                sizeof(uint32_t));
    current_offset += sizeof(uint32_t);
  } else {
    payload_header.PTS = 0;
  }

  if (payload_header.bmBFH.BFH_SCR &&
      current_offset + sizeof(uint64_t) <= uvc_payload.size()) {
    // scr if present
    std::memcpy(&payload_header.SCR, &uvc_payload[current_offset],
                sizeof(uint64_t));
  } else {
    payload_header.SCR = 0;
  }

  //save_payload_header_to_log(payload_header, received_time);

  return payload_header;
}

UVCError UVCPHeaderChecker::payload_header_valid(
    const UVC_Payload_Header& payload_header,
    const UVC_Payload_Header& previous_payload_header,
    const UVC_Payload_Header& previous_previous_payload_header) {
  // Checks if the Error bit is set
  if (payload_header.bmBFH.BFH_ERR) {
    v_cerr_2 << "Invalid UVC payload header: Error bit is set." << std::endl;
    return ERR_ERR_BIT_SET;
  }

  // Checks if the header length is valid
  if (payload_header.HLE < 0x02 || payload_header.HLE > 0x0C) {
    v_cerr_2 << "Invalid UVC payload header: Unexpected start byte 0x"
             << std::hex << std::setw(2) << std::setfill('0')
             << static_cast<int>(payload_header.HLE) << "." << std::endl;
    return ERR_LENGTH_OUT_OF_RANGE; 
  }

  // Checks if the Presentation Time Stamp bit is set
  // Checks if the Source Clock Reference bit is set
  if (payload_header.bmBFH.BFH_PTS && payload_header.bmBFH.BFH_SCR &&
      payload_header.HLE != 0x0C) {
    v_cerr_2 << "Invalid UVC payload header: Both Presentation Time Stamp and "
                "Source Clock Reference bits are set."
             << std::endl;
    return ERR_LENGTH_INVALID;
  } else if (payload_header.bmBFH.BFH_PTS && !payload_header.bmBFH.BFH_SCR &&
             payload_header.HLE != 0x06) {
    v_cerr_2 << "Invalid UVC payload header: Presentation Time Stamp bit is "
                "set but header length is less than 6."
             << std::endl;
    return ERR_LENGTH_INVALID;
  } else if (!payload_header.bmBFH.BFH_PTS && payload_header.bmBFH.BFH_SCR &&
             payload_header.HLE != 0x08) {
    v_cerr_2 << "Invalid UVC payload header: Source Clock Reference bit is "
                "set but header length is less than 12."
             << std::endl;
    return ERR_LENGTH_INVALID;
  } else if (!payload_header.bmBFH.BFH_PTS && !payload_header.bmBFH.BFH_SCR &&
             payload_header.HLE != 0x02) {
    v_cerr_2
        << "Invalid UVC payload header: Neither Presentation Time Stamp nor "
           "Source Clock Reference bits are set but header length is not 2."
        << std::endl;
    return ERR_LENGTH_INVALID;
  }

  // Check with the total length of the frame and the calculated length of the
  // frame
  if (payload_header.bmBFH.BFH_EOF) {
  } else {
    if (payload_header.bmBFH.BFH_RES) {
      v_cerr_2 << "Invalid UVC payload header: Reserved bit is set."
               << std::endl;
      return ERR_RESERVED_BIT_SET;
    }
  }

  // Checks if the Frame Identifier bit is set
  if (payload_header.bmBFH.BFH_FID == previous_payload_header.bmBFH.BFH_FID) {
    if (previous_payload_header.bmBFH.BFH_EOF) {
      if ((payload_header.PTS == previous_payload_header.PTS) &&
          payload_header.PTS != 0) {
        v_cerr_2 << "Invalid UVC payload header: Frame Identifier bit is same "
                    "as the previous frame."
                 << std::endl;
        return ERR_SWAP;
      }
      return ERR_FID_MISMATCH;
    }
  } else {
    if (!previous_payload_header.bmBFH.BFH_EOF &&
        previous_payload_header.HLE != 0) {
      v_cerr_2 << "Missing EOF" << std::endl;
      return ERR_MISSING_EOF;  // missing frame for bulk for whole
    }
  }

  // Checks if the Still Image bit is set is not needed

  // //Checks if the End of Header bit is set 0 for iso and 1 for bulk
  // if (!payload_header.bmBFH.BFH_EOH) {
  //     v_cerr_2 << "Invalid UVC payload header: End of Header (EOH) bit is
  //     not set." << std::endl; return 1;
  // }

  // v_cout_2 << "UVC payload header is valid." << std::endl;
  return ERR_NO_ERROR;
}

void UVCPHeaderChecker::frame_valid_ctrl(
    const std::vector<u_char>& uvc_payload) {
  // Save the format of the frame
  // Save them on the multiple stack with the time, so err frame be detected and
  // pop from the stack The algorithm to detect the error frame is to compare
  // the frame with the previous frame
}

void UVCPHeaderChecker::payload_frame_develope() {
  // Call the picture file
  // DO NOT NEED THIS FOR NOW
}

void UVCPHeaderChecker::save_frames_to_log(
    std::unique_ptr<ValidFrame>& current_frame) {
  std::ofstream log_file("../log/frames_log.txt", std::ios::app);

  if (!log_file.is_open()) {
    v_cerr_2 << "Error opening log file." << std::endl;
    return;
  }

  std::stringstream frame_info;
  frame_info << "Frame Number: " << current_frame->frame_number << "\n"
             << "Packet Number: " << current_frame->packet_number << "\n"
             << "Frame PTS: " << current_frame->frame_pts << "\n"
             << "Frame Error: " << static_cast<int>(current_frame->frame_error)
             << "\n"
             << "EOF Reached: " << static_cast<int>(current_frame->eof_reached)
             << "\n"
             << "Payloads:\n";

  for (size_t i = 0; i < current_frame->payload_headers.size(); ++i) {
    const UVC_Payload_Header& header = current_frame->payload_headers[i];
    size_t payload_size = current_frame->payload_sizes[i];

    // Get the time point from received_chrono_times
    auto time_point = current_frame->received_chrono_times[i];
    auto duration_since_epoch = time_point.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                            duration_since_epoch)
                            .count();

    frame_info
        << "  Payload " << i + 1 << ":\n"
        << "    Payload Size: " << payload_size << " bytes\n"
        << "    Received Time: " << milliseconds
        << " ms since epoch\n";  // Logging the received time in milliseconds
  }

  log_file << frame_info.str() << "\n---\n";

  log_file.close();
}

void UVCPHeaderChecker::save_payload_header_to_log(
    const UVC_Payload_Header& payload_header,
    std::chrono::time_point<std::chrono::steady_clock> received_time) {

#ifdef __linux__
  std::ofstream log_file("../log/payload_headers_log.txt", std::ios::app);
#elif _WIN32
  std::ofstream log_file("..\\..\\log\\payload_headers_log.txt", std::ios::app);
#endif

  if (!log_file.is_open()) {
    v_cerr_2 << "Error opening payload header log file." << std::endl;
    return;
  }

  log_file << std::hex;  // Set the output stream to hexadecimal mode
  log_file << "HLE: " << std::setw(2) << std::setfill('0')
           << static_cast<int>(payload_header.HLE) << "\n";

  // Log each bit field separately
  log_file << "BFH:\n"
           << "  FID: " << std::bitset<1>(payload_header.bmBFH.BFH_FID) << "\n"
           << "  EOF: " << std::bitset<1>(payload_header.bmBFH.BFH_EOF) << "\n"
           << "  PTS: " << std::bitset<1>(payload_header.bmBFH.BFH_PTS) << "\n"
           << "  SCR: " << std::bitset<1>(payload_header.bmBFH.BFH_SCR) << "\n"
           << "  RES: " << std::bitset<1>(payload_header.bmBFH.BFH_RES) << "\n"
           << "  STI: " << std::bitset<1>(payload_header.bmBFH.BFH_STI) << "\n"
           << "  ERR: " << std::bitset<1>(payload_header.bmBFH.BFH_ERR) << "\n"
           << "  EOH: " << std::bitset<1>(payload_header.bmBFH.BFH_EOH) << "\n";

  log_file << "PTS: " << std::setw(8) << std::setfill('0') << payload_header.PTS
           << "\n";
  log_file << "SCR:\n"
           << "  STC: " << static_cast<int>(payload_header.bmSCR.SCR_STC)
           << "\n"
           << "  TOK: " << std::bitset<11>(payload_header.bmSCR.SCR_TOK) << "\n"
           << "  RES: " << std::bitset<5>(payload_header.bmSCR.SCR_RES) << "\n";

  log_file << std::dec  // Set the output stream back to decimal mode
           << "Received Time: "
           << std::chrono::duration_cast<std::chrono::milliseconds>(
                  received_time.time_since_epoch())
                  .count()
           << " ms since epoch\n"
           << "\n\n";  // Separate each entry with a double newline

  log_file.close();
}
