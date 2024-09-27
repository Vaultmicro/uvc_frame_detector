
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <cstddef>

#include "validuvc/uvcpheader_checker.hpp"
#include "validuvc/control_config.hpp"
#include "utils/verbose.hpp"


uint8_t UVCPHeaderChecker::payload_valid_ctrl(
    const std::vector<u_char>& uvc_payload,
    std::chrono::time_point<std::chrono::steady_clock> received_time) {
  // Make graph file
  // Make picture file having mjpeg, yuyv, h264

  if (uvc_payload.empty()) {
    // v_cerr_2 << "UVC payload is empty." << std::endl;
    return 0;
  }

  static UVC_Payload_Header previous_previous_payload_header;
  static uint8_t previous_previous_error = 0;
  static UVC_Payload_Header previous_payload_header;
  static uint8_t previous_error = 0;
  // static std::queue<std::vector<u_char>> fid_queue;

  UVC_Payload_Header payload_header = parse_uvc_payload_header(uvc_payload);

  uint8_t payload_header_valid_return =
      payload_header_valid(payload_header, previous_payload_header,
                           previous_previous_payload_header);

  bool frame_found = false;

  for (auto& frame : frames) {
    if (frame->frame_pts == payload_header.PTS && payload_header.PTS) {
      frame_found = true;
      frame->add_packet(uvc_payload);// if frame found, add packet to the frame
      frame->add_received_chrono_time(received_time);

      if (payload_header_valid_return) {
        frame->set_frame_error();  // if error set, add error flag to the frame
      }

      break;
    }
  }

  // create new frame if not found
  if (!frame_found || previous_payload_header.bmBFH.BFH_EOF) {
    frames.push_back(std::make_unique<ValidFrame>(frames.size() + 1));
    auto& new_frame = frames.back();
    new_frame->frame_pts = payload_header.PTS;  // frame pts == payload pts
    new_frame->add_packet(uvc_payload);
    new_frame->add_received_chrono_time(received_time);

    if (payload_header_valid_return) {
      new_frame->set_frame_error();
    }
  }

  // Check if the previous frame has an error and switch
  if (!previous_error) {
    previous_payload_header = payload_header;
  }

  if (payload_header.bmBFH.BFH_EOF) {
    // finish the frame
    save_frames_to_log(frames.back());
    if (!frames.back()->frame_error) {
      frames.pop_back();
      frame_count++;
    } else {
      // save them on error heap
    }
  }

  if (payload_header_valid_return) {
    // TODO save in the error frame heap
    return payload_header_valid_return;
  }

  uint8_t previous_fid = payload_header.bmBFH.BFH_FID;

  //v_cout_2 << "Payload is valid." << std::endl;

  return 0;
}

void UVCPHeaderChecker::timer_thread() {
    while (!stop_timer_thread) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); 
        std::cout << "FPS: " << frame_count.load() << " frames per second" << std::endl;
        frame_count = 0;
    }
}

UVC_Payload_Header UVCPHeaderChecker::parse_uvc_payload_header(
    const std::vector<u_char>& uvc_payload) {
  UVC_Payload_Header payload_header;
  if (uvc_payload.size() < 2) {
    v_cerr_2 << "Error: UVC payload size is too small." << std::endl;
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

  save_payload_header_to_log(payload_header);

  return payload_header;
}

uint8_t UVCPHeaderChecker::payload_header_valid(
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
              << static_cast<int>(payload_header.HLE) << "."
              << std::endl;
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

  // Checks if the End of Frame bit is set
  // TODO
  // Check with the total length of the frame and the calculated length of the
  // frame
  if (payload_header.bmBFH.BFH_EOF) {
    // if (previous_payload_header.bmBFH.BFH_EOF &&
    // (payload_header.bmBFH.BFH_FID == previous_payload_header.bmBFH.BFH_FID)){
    //     v_cerr_2 << "Invalid UVC payload header: Missing frame for bulk
    //     whole." << std::endl; return 1;
    // }
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
      if ((payload_header.PTS != previous_payload_header.PTS) &&
          payload_header.PTS != 0) {
        v_cerr_2 << "Invalid UVC payload header: Frame Identifier bit is same "
                     "as the previous frame."
                  << std::endl;
        return ERR_FID_MISMATCH;
      }
    }
  }

  // Checks if the Still Image bit is set is not needed

  // //Checks if the End of Header bit is set 0 for iso and 1 for bulk
  // if (!payload_header.bmBFH.BFH_EOH) {
  //     v_cerr_2 << "Invalid UVC payload header: End of Header (EOH) bit is
  //     not set." << std::endl; return 1;
  // }

  v_cout_2 << "UVC payload header is valid." << std::endl;
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
             << "Packets:\n";

  for (const auto& packet : current_frame->packets) {
    frame_info << "  Packet Size: " << packet.size() << " bytes\n";
  }

  log_file << frame_info.str() << "\n---\n";

  log_file.close();
}

void UVCPHeaderChecker::save_payload_header_to_log(
    const UVC_Payload_Header& payload_header) {
  std::ofstream log_file("../log/payload_headers_log.txt", std::ios::app);

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
           << "  RES: " << std::bitset<5>(payload_header.bmSCR.SCR_RES) << "\n"
           << "\n\n";  // Separate each entry with a double newline

  log_file.close();
}


void UVCPHeaderChecker::print_packet(const std::vector<u_char>& packet) {
  for (size_t i = 0; i < packet.size(); i += 16) {
    // printf("%08zx: ", i);

    for (size_t j = 0; j < 16; ++j) {
      if (i + j < packet.size()) {
        printf("%02x ", packet[i + j]);
      } else {
        printf("   ");  // padding
      }
    }

    printf(" ");
    // for (size_t j = 0; j < 16; ++j) {
    //     if (i + j < packet.size()) {
    //         unsigned char ch = packet[i + j];
    //         printf("%c", isprint(ch) ? ch : '.');
    //     }
    // }
    printf("\n");
  }
}
