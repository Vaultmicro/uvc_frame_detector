﻿/*********************************************************************
 * Copyright (c) 2024 Vaultmicro, Inc
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*********************************************************************/


#include <chrono>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <cstddef>
#include <stddef.h>
#include <algorithm>

#include "utils/verbose.hpp"
#include "validuvc/uvcpheader_checker.hpp"
#include "validuvc/control_config.hpp"

#ifdef TUI_SET
#include "utils/tui_win.hpp"
#elif GUI_SET
#include "gui_win.hpp"
#endif

#ifdef _WIN32
  typedef unsigned char u_char;
#endif

int UVCPHeaderChecker::continue_capture = 1;

uint8_t UVCPHeaderChecker::payload_valid_ctrl(
    const std::vector<u_char>& uvc_payload,
    std::chrono::time_point<std::chrono::steady_clock> received_time) {
  // Make picture file having mjpeg, yuyv, h264

  static std::chrono::time_point<std::chrono::steady_clock> temp_received_time;
  static std::chrono::time_point<std::chrono::steady_clock> temp_received_time_graph;

  received_time_clock = std::chrono::duration_cast<std::chrono::milliseconds>(received_time.time_since_epoch()).count();
  formatted_time = formatTime(std::chrono::milliseconds(received_time_clock));

#ifdef TUI_SET
  window_number = 1;
#elif GUI_SET
  temp_window_number = gui_window_number;
  gui_window_number = 5;
#endif

  if (uvc_payload.empty()) {          
    v_cerr_3 << "UVC payload is empty." << std::endl;
    update_payload_error_stat(ERR_EMPTY_PAYLOAD);
    return ERR_EMPTY_PAYLOAD;
  }
  if (uvc_payload.size() > ControlConfig::dwMaxPayloadTransferSize) {

    v_cerr_3 << "UVC payload size exceeds maximum transfer size." << std::endl;

    update_payload_error_stat(ERR_MAX_PAYLAOD_OVERFLOW);
    return ERR_MAX_PAYLAOD_OVERFLOW;
  }

  static uint64_t received_frames_cnt = 0;
  static uint64_t received_throughput = 0;  
  
  auto pass_time_count = std::chrono::duration_cast<std::chrono::seconds>(received_time - temp_received_time).count();

  if (temp_received_time == std::chrono::time_point<std::chrono::steady_clock>()) {
    temp_received_time = received_time;
  } else if (pass_time_count >= 1){

    if (pass_time_count >= 2) {
        for (int i = 1; i < pass_time_count; ++i) {
            frame_count = 0;
            throughput = 0;

            int fps_difference = ControlConfig::fps - frame_count;
            if (frame_count != ControlConfig::fps) {
                frame_stats.count_frame_drop += fps_difference;
            }
            average_frame_rate = (average_frame_rate * received_frames_cnt + frame_count) / (received_frames_cnt + 1);
            received_frames_cnt++;
    #ifdef TUI_SET
            window_number = 2;
    #elif GUI_SET
            temp_window_number = gui_window_number;
            gui_window_number = 9;
    #endif
            v_cout_1 << "FPS: " << frame_count << " : " << formatted_time << std::endl;
    #ifdef GUI_SET
            gui_window_number = 10;
    #endif
    #ifndef TUI_SET
            v_cout_1 << "Throughput: " << throughput * 8 / 1000000 << " mbps  " << formatted_time << std::endl;
    #endif
    #ifdef TUI_SET
            window_number = 1;
    // #elif GUI_SET
    //         WindowManager& manager = WindowManager::getInstance();
    //         GraphData& data = manager.getGraphData(0);
    //         data.addGraphData(0.0f);
    //         gui_window_number = 5;
    #endif
    #if defined(TUI_SET) || defined(GUI_SET)
            print_stats();
    #endif
            temp_received_time += std::chrono::seconds(1);
        }
    }

#ifdef TUI_SET
  window_number = 2;
#elif GUI_SET
  temp_window_number = gui_window_number;
  gui_window_number = 9;
#endif
    v_cout_1 << "FPS: " << frame_count << " : " << formatted_time << std::endl;

#ifdef GUI_SET
  gui_window_number = 10;
#endif

#ifndef TUI_SET
    v_cout_1 << "Throughput: " << throughput * 8 / 1000000 << " mbps  " << formatted_time <<std::endl;
#endif

#ifdef TUI_SET
    window_number = 1;
#elif GUI_SET
//   WindowManager& manager = WindowManager::getInstance();
//   GraphData& data = manager.getGraphData(0);
//   data.addGraphData(static_cast<float>(throughput * 8));
  gui_window_number = 5;
#endif

    int fps_difference = ControlConfig::fps - frame_count;
    if (frame_count != ControlConfig::fps){
      frame_stats.count_frame_drop += fps_difference;
    }

    average_frame_rate = (average_frame_rate * received_frames_cnt + frame_count)/(received_frames_cnt + 1);
    received_frames_cnt++;

    frame_count = 0;
    temp_received_time = received_time - std::chrono::milliseconds(1);
    throughput = 0;

#if defined(TUI_SET) || defined(GUI_SET)
    print_stats();
#endif
  }

  graph_throughput += uvc_payload.size();
  throughput += uvc_payload.size();


  static UVC_Payload_Header previous_previous_payload_header;
  static UVC_Payload_Header previous_payload_header;
  static UVC_Payload_Header temp_error_payload_header;


  UVC_Payload_Header payload_header =
      parse_uvc_payload_header(uvc_payload, received_time);

  UVCError payload_header_valid_return =
      payload_header_valid(payload_header, previous_payload_header,
                           previous_previous_payload_header);

  static uint32_t previous_frame_pts = 0;

  if (!payload_header_valid_return || payload_header_valid_return == ERR_MISSING_EOF) {

    //Process the last frame when EOF is missing
    if (payload_header_valid_return == ERR_MISSING_EOF) {
      v_cerr_3 << " : Missing EOF." << std::endl;
      if (!frames.empty()) {
        auto& last_frame = frames.back();
        last_frame->frame_error = ERR_FRAME_MISSING_EOF;
        last_frame->eof_reached = false;
        //finish the last frame
        update_frame_error_stat(last_frame->frame_error);
        //save_frames_to_log(last_frame);
        if (last_frame->frame_error) {
#ifdef GUI_SET
          addErrorFrameLog("Frame " + std::to_string(last_frame->frame_number));
          WindowManager& manager = WindowManager::getInstance();
          GraphData& data = manager.getGraphData(0);
          data.addErrorGraphData();
          // error_frame_log_button.push_back("Frame " + std::to_string(last_frame->frame_number));
          frame_error_flag = 1;
          print_received_times(*last_frame);
          print_frame_data(*last_frame);
          print_summary(*last_frame);
          print_error_bits(previous_payload_header, temp_error_payload_header ,payload_header);
          {
            WindowData& data = manager.getWindowData(6);
            data.add_button_log_text();
          }
          {
            WindowData& data = manager.getWindowData(7);
            data.add_button_log_text();
          }
          {
            WindowData& data = manager.getWindowData(8);
            data.add_button_log_text();
          }
          frame_error_flag = 0;
#else
          plot_received_chrono_times(last_frame->received_chrono_times, last_frame->received_error_times);
          print_error_bits(previous_payload_header, temp_error_payload_header ,payload_header);
#endif
        }
        if (continue_capture){
          last_frame->push_queue();
        }
        processed_frames.push_back(std::move(frames.back()));
        frames.pop_back();
        frame_count++;

        if (processed_frames.size() > 3) {
          processed_frames.erase(processed_frames.begin());
        }

      }
    }

    bool frame_found = false;

    for (auto& frame : frames) {
      if (previous_payload_header.bmBFH.BFH_FID == payload_header.bmBFH.BFH_FID) {
        frame_found = true;

        if (payload_header.PTS){          
          if (!previous_frame_pts) {
            previous_frame_pts = payload_header.PTS;
          } else if (payload_header.PTS == previous_frame_pts){
            
          } else if (payload_header.PTS != previous_frame_pts) {
            frame->frame_pts = payload_header.PTS;  // frame pts == payload pts
          }
        }

        frame->add_payload(payload_header, uvc_payload.size(), uvc_payload);
        frame->add_received_chrono_time(received_time);

#ifdef GUI_SET
        WindowManager& manager = WindowManager::getInstance();
        GraphData& data = manager.getGraphData(0);
        data.addGraphData(static_cast<float>(uvc_payload.size()));
#endif

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

      ++current_frame_number;
      frames.push_back(std::make_unique<ValidFrame>(current_frame_number));
      auto& new_frame = frames.back();

      new_frame->toggle_bit = payload_header.bmBFH.BFH_FID;

      if (payload_header.PTS){
        if (!previous_frame_pts) {
          previous_frame_pts = payload_header.PTS;
        } else if (payload_header.PTS == previous_frame_pts){
          
        } else if (payload_header.PTS != previous_frame_pts) {
          new_frame->frame_pts = payload_header.PTS;  // frame pts == payload pts
        }
      }

      new_frame->add_payload(payload_header, uvc_payload.size(), uvc_payload);
      new_frame->add_received_chrono_time(received_time);
      new_frame->set_frame_format(ControlConfig::get_width(), ControlConfig::get_height(), ControlConfig::get_frame_format());

#ifdef GUI_SET
        WindowManager& manager = WindowManager::getInstance();
        GraphData& data = manager.getGraphData(0);
        data.custom_text = "Frame Number: " + std::to_string(current_frame_number);
        data.graph_reset();
        data.addGraphData(static_cast<float>(uvc_payload.size()));
#endif

      size_t total_payload_size = std::accumulate(new_frame->payload_sizes.begin(), new_frame->payload_sizes.end(), size_t(0));
      if (total_payload_size > ControlConfig::dwMaxVideoFrameSize) {
        new_frame->frame_error = ERR_FRAME_MAX_FRAME_OVERFLOW;  
      }

      if (payload_header_valid_return && payload_header_valid_return != ERR_MISSING_EOF) {
        new_frame->set_frame_error();
      }
    }


    //add image data here
    auto& last_frame = frames.back();
    last_frame->add_image_data(payload_header, uvc_payload);



    if (payload_header.bmBFH.BFH_EOF) {

      auto& last_frame = frames.back();
      last_frame->eof_reached = true;

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
        for (size_t i = 0; i < last_frame->payload_sizes.size(); ++i) {
          actual_frame_size += last_frame->payload_sizes[i];
        }

        if (actual_frame_size != expected_frame_size) {
          v_cerr_2 << "Frame size mismatch for YUYV: \n expected "
                  << std::dec
                  << expected_frame_size << " but got " << actual_frame_size
                  << std::endl;
          last_frame->frame_error = ERR_FRAME_INVALID_YUYV_RAW_SIZE;
        }

      }

      update_frame_error_stat(last_frame->frame_error);
      // finish the frame
      // save_frames_to_log(frames.back());
      if (last_frame->frame_error) {
#ifdef GUI_SET
        addErrorFrameLog("Frame " + std::to_string(last_frame->frame_number));
        WindowManager& manager = WindowManager::getInstance();
        GraphData& data = manager.getGraphData(0);
        data.addErrorGraphData();
        // error_frame_log_button.push_back("Frame " + std::to_string(last_frame->frame_number));
        frame_error_flag = 1;
        print_received_times(*last_frame);
        print_frame_data(*last_frame);
        print_summary(*last_frame);
        print_error_bits(previous_payload_header, temp_error_payload_header ,payload_header);
          {
            WindowData& data = manager.getWindowData(6);
            data.add_button_log_text();
          }
          {
            WindowData& data = manager.getWindowData(7);
            data.add_button_log_text();
          }
          {
            WindowData& data = manager.getWindowData(8);
            data.add_button_log_text();
          }
        frame_error_flag = 0;

        // develope frame image here

#else
        plot_received_chrono_times(last_frame->received_chrono_times, last_frame->received_error_times);
        print_error_bits(previous_payload_header, temp_error_payload_header ,payload_header);
#endif
        if (continue_capture){
          last_frame->push_queue();
        }
      } else{
#ifdef GUI_SET
        print_frame_data(*last_frame);
#endif      
      }
      processed_frames.push_back(std::move(frames.back()));
      frames.pop_back();
      frame_count++;

      if (processed_frames.size() > 3) {
        processed_frames.erase(processed_frames.begin());
      }

    }
    

    previous_payload_header = payload_header;
    previous_previous_payload_header = previous_payload_header;
    temp_error_payload_header = {};
    p_formatted_time = formatted_time;
    e_formatted_time = "";

    update_payload_error_stat(payload_header_valid_return);

    return payload_header_valid_return;

  } else {
    // handle payload header errors
    if (!frames.empty()) {
      auto& last_frame = frames.back();
      last_frame->frame_error = ERR_FRAME_ERROR;
      last_frame->add_received_error_time(received_time);
      last_frame->payload_sizes.push_back(uvc_payload.size());
      last_frame->payload_errors.push_back(payload_header_valid_return);
      last_frame->lost_data_sizes.push_back(uvc_payload.size());
      last_frame->packet_number++;

      // print_frame_data(*last_frame);
#ifndef GUI_SET
      plot_received_chrono_times(last_frame->received_chrono_times, last_frame->received_error_times);
#endif
    }
    print_error_bits(previous_payload_header, temp_error_payload_header ,payload_header);

    temp_error_payload_header = payload_header;
    e_formatted_time = formatted_time;

    update_payload_error_stat(payload_header_valid_return);
// #ifdef GUI_SET
//     printUVCErrorExplanation(payload_header_valid_return);
// #endif
    return payload_header_valid_return;
  }

  // v_cout_5 << "Payload is valid." << std::endl;
  update_payload_error_stat(ERR_UNKNOWN);
  return ERR_UNKNOWN;
}




void UVCPHeaderChecker::frame_valid_ctrl(
    const std::vector<u_char>& uvc_payload) {

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
    current_offset += sizeof(uint64_t);
  } else {
    payload_header.SCR = 0;
  }

  // if (current_offset < uvc_payload.size()) {
  //   payload.assign(uvc_payload.begin() + current_offset, uvc_payload.end());
  // } else {
  //   payload.clear();
  // }

  //save_payload_header_to_log(payload_header, received_time);

  return payload_header;
}

UVCError UVCPHeaderChecker::payload_header_valid(
    const UVC_Payload_Header& payload_header,
    const UVC_Payload_Header& previous_payload_header,
    const UVC_Payload_Header& previous_previous_payload_header) {

  // Checks if the Error bit is set
  if (payload_header.bmBFH.BFH_ERR) {
    v_cerr_2 << " : Error bit is set." << formatted_time << std::endl;
    return ERR_ERR_BIT_SET;
  }

  // Checks if the header length is valid
  if (payload_header.HLE < 0x02 || payload_header.HLE > 0x0C) {
    v_cerr_2 << " : Unexpected start byte 0x"
             << std::hex << std::setw(2) << std::setfill('0')
             << static_cast<int>(payload_header.HLE) << "." << formatted_time << std::endl;
    return ERR_LENGTH_OUT_OF_RANGE; 
  }

  // Checks if the Presentation Time Stamp bit is set
  // Checks if the Source Clock Reference bit is set
  if (payload_header.bmBFH.BFH_PTS && payload_header.bmBFH.BFH_SCR &&
      payload_header.HLE != 0x0C) {
    v_cerr_2 << " : Both Presentation Time Stamp and "
                "Source Clock Reference bits are set." << formatted_time
             << std::endl;
    return ERR_LENGTH_INVALID;
  } else if (payload_header.bmBFH.BFH_PTS && !payload_header.bmBFH.BFH_SCR &&
             payload_header.HLE != 0x06) {
    v_cerr_2 << " : Presentation Time Stamp bit is "
                "set but header length is less than 6." << formatted_time
             << std::endl;
    return ERR_LENGTH_INVALID;
  } else if (!payload_header.bmBFH.BFH_PTS && payload_header.bmBFH.BFH_SCR &&
             payload_header.HLE != 0x08) {
    v_cerr_2 << " : Source Clock Reference bit is "
                "set but header length is less than 12." << formatted_time
             << std::endl;
    return ERR_LENGTH_INVALID;
  } else if (!payload_header.bmBFH.BFH_PTS && !payload_header.bmBFH.BFH_SCR &&
             payload_header.HLE != 0x02) {
    v_cerr_2
        << " : Neither Presentation Time Stamp nor "
           "Source Clock Reference bits are set but header length is not 2." << formatted_time
        << std::endl;
    return ERR_LENGTH_INVALID;
  }

  // Check with the total length of the frame and the calculated length of the
  // frame
  if (payload_header.bmBFH.BFH_EOF) {
  } else {
    if (payload_header.bmBFH.BFH_RES) {
      v_cerr_2 << " : Reserved bit is set." << formatted_time
               << std::endl;
      return ERR_RESERVED_BIT_SET;
    }
  }

  // if (payload_header.bmSCR.SCR_STC != 0 && previous_payload_header.bmSCR.SCR_STC != 0 &&
  //     payload_header.bmSCR.SCR_STC < previous_payload_header.bmSCR.SCR_STC &&
  //     (previous_payload_header.bmSCR.SCR_STC - payload_header.bmSCR.SCR_STC) < 0x80000000) {
  //   v_cerr_2 << " : STC decreased." << formatted_time << std::endl;
  //   return ERR_TOGGLE_BIT_OVERLAPPED;
  // }

  // if (payload_header.PTS != 0 && previous_payload_header.PTS != 0 &&
  //     payload_header.PTS < previous_payload_header.PTS) {
  //   v_cerr_2 << " : PTS is less than previous PTS." << formatted_time << std::endl;
  //   return ERR_TOGGLE_BIT_OVERLAPPED;
  // }

  // Checks if the Frame Identifier bit is set
  // bmBFH.BFH_FID is for the start of the stream packet
  if (payload_header.bmBFH.BFH_FID == previous_payload_header.bmBFH.BFH_FID && 
        previous_payload_header.bmBFH.BFH_EOF && 
        (payload_header.PTS == previous_payload_header.PTS) && 
        payload_header.PTS != 0) {
        v_cerr_2 << " : Same FID "
                    "and prev frame and PTS matches0. " << formatted_time << std::endl;
        return ERR_SWAP;

  } else if (payload_header.bmBFH.BFH_FID == previous_payload_header.bmBFH.BFH_FID && 
            previous_payload_header.bmBFH.BFH_EOF &&  previous_payload_header.HLE !=0) {
      v_cerr_2 << " : Same FID "
                  "and prev frame EOF is set." << formatted_time << std::endl;
      return ERR_FID_MISMATCH;

  } else if (payload_header.bmBFH.BFH_FID != previous_payload_header.bmBFH.BFH_FID && 
            !previous_payload_header.bmBFH.BFH_EOF && 
            previous_payload_header.HLE != 0) {
      v_cerr_2 << " : Missing EOF.   " << formatted_time << std::endl;
      return ERR_MISSING_EOF;      
  } 

  // Checks if the Still Image bit is set is not needed

  // //Checks if the End of Header bit is set 0 for iso and 1 for bulk
  // if (!payload_header.bmBFH.BFH_EOH) {
  //     v_cerr_2 << " : End of Header (EOH) bit is
  //     not set." << std::endl; return 1;
  // }

  // v_cout_2 << "UVC payload header is valid." << std::endl;
  return ERR_NO_ERROR;
}

void UVCPHeaderChecker::save_frames_to_log(
    std::unique_ptr<ValidFrame>& current_frame) {
  std::ofstream log_file("../log/frames_log.txt", std::ios::app);

  if (!log_file.is_open()) {
    v_cerr_5 << "Error opening log file." << std::endl;
    return;
  }

  std::stringstream frame_info;
  frame_info << "Frame Number: " << current_frame->frame_number << "\n"
             << "Toggle Bit (FID): " << static_cast<int>(current_frame->toggle_bit) << "\n"
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

//making graph

void UVCPHeaderChecker::save_payload_header_to_log(
    const UVC_Payload_Header& payload_header,
    std::chrono::time_point<std::chrono::steady_clock> received_time) {

#ifdef __linux__
  std::ofstream log_file("../log/payload_headers_log.txt", std::ios::app);
#elif _WIN32
  std::ofstream log_file("..\\..\\log\\payload_headers_log.txt", std::ios::app);
#endif

  if (!log_file.is_open()) {
    v_cerr_3 << "Error opening payload header log file." << std::endl;
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

void UVCPHeaderChecker::print_error_bits(const UVC_Payload_Header& previous_payload_header, const UVC_Payload_Header& temp_error_payload_header, const UVC_Payload_Header& payload_header) {
    // v_cout_2 << "Frame Error Type__: " << frame_error << std::endl;

#ifdef TUI_SET
  window_number = 4;
  print_whole_flag = 1;
    v_cout_2 << "Previous Payload Header: " <<  "\n";
#elif GUI_SET
  frame_error_flag = 1;
  gui_window_number = 6;
  print_whole_flag = 1;
#endif
    v_cout_2 << previous_payload_header << "\n" << p_formatted_time <<  std::endl;
#ifdef TUI_SET
  window_number = 5;
    v_cout_2 << "Lost Inbetween Header: " <<  "\n";
#elif GUI_SET
  gui_window_number = 7;
#endif
    v_cout_2 << temp_error_payload_header << "\n" << e_formatted_time <<  std::endl;
#ifdef TUI_SET
  window_number = 6;
    v_cout_2 << "Current Payload Header: " <<  "\n";
#elif GUI_SET
  gui_window_number = 8;
#endif
    v_cout_2 << payload_header << "\n" << formatted_time <<  std::endl;
#ifdef TUI_SET
  window_number = 1;
  print_whole_flag = 0;
#elif GUI_SET
  gui_window_number = 5;
  print_whole_flag = 0;
  frame_error_flag = 0;
#else
    v_cout_2 <<  std::endl;
#endif
}


std::ostream& operator<<(std::ostream& os, const UVC_Payload_Header& header) {
    os << "HLE: " << static_cast<int>(header.HLE) << "\n";
    
    os << "BFH: 0x" << std::bitset<8>(header.BFH) << "\n";
    os << "  BFH_FID: " << static_cast<int>(header.bmBFH.BFH_FID) << "\n";
    os << "  BFH_EOF: " << static_cast<int>(header.bmBFH.BFH_EOF) << "\n";
    os << "  BFH_PTS: " << static_cast<int>(header.bmBFH.BFH_PTS) << "\n";
    os << "  BFH_SCR: " << static_cast<int>(header.bmBFH.BFH_SCR) << "\n";
    os << "  BFH_RES: " << static_cast<int>(header.bmBFH.BFH_RES) << "\n";
    os << "  BFH_STI: " << static_cast<int>(header.bmBFH.BFH_STI) << "\n";
    os << "  BFH_ERR: " << static_cast<int>(header.bmBFH.BFH_ERR) << "\n";
    os << "  BFH_EOH: " << static_cast<int>(header.bmBFH.BFH_EOH) << "\n";

    os << "PTS: 0x" << std::setw(8) << std::setfill('0') << std::hex << header.PTS 
       << " \n    (" << std::dec << header.PTS << ")\n";

    os << "SCR: 0x" << std::hex << header.SCR << std::dec << "\n";
  
    os << "  SCR_STC: 0x" << std::setw(8) << std::setfill('0') << std::hex << header.bmSCR.SCR_STC 
       << " \n        (" << std::dec << header.bmSCR.SCR_STC << ")\n";
    os << "  SCR_TOK: " << std::bitset<11>(header.bmSCR.SCR_TOK) << "\n";
    os << "  SCR_RES: " << std::bitset<5>(header.bmSCR.SCR_RES) << "\n";

    return os;
}

void UVCPHeaderChecker::plot_received_chrono_times(const std::vector<std::chrono::steady_clock::time_point>& received_chrono_times, 
                                                    const std::vector<std::chrono::steady_clock::time_point>& received_error_times) {
                                                      
    if (received_chrono_times.empty() && received_error_times.empty()) return;

#ifdef TUI_SET
  window_number = 7;
#endif

    const int zoom = 4;
    const int cut = 20;
    const int total_markers = ControlConfig::fps * zoom;         
    const auto interval_ns = std::chrono::nanoseconds(static_cast<long long>(1e9 / static_cast<double>(ControlConfig::fps) / (zoom *cut)));


    auto base_time = received_chrono_times[0];

    std::string graph(total_markers, '_');

    for (const auto& time_point : received_chrono_times) {
        auto time_diff_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(time_point - base_time);

        int position = static_cast<int>(time_diff_ns.count() / interval_ns.count());

        if (position < total_markers) {
            graph[position] = 'o';
        }
    }
    for (const auto& time_point : received_error_times) {
        auto time_diff_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(time_point - base_time);

        int position = static_cast<int>(time_diff_ns.count() / interval_ns.count());

        if (position < total_markers) {
            graph[position] = 'x';
        }
    }
    

    // v_cout_2 << "Graph Data (Payload Reception Times): " << std::endl;
    v_cout_2 << graph << formatted_time << std::endl;

#ifdef TUI_SET
  window_number = 1;
#endif
}

void UVCPHeaderChecker::print_received_times(const ValidFrame& frame) {
#ifdef GUI_SET
    gui_window_number = 1;
#endif
    // Vector to store all times and labels
    std::vector<std::pair<std::chrono::time_point<std::chrono::steady_clock>, std::string>> all_times;

    // Populate vector with both chrono times and error times along with labels
    for (const auto& time_point : frame.received_chrono_times) {
        all_times.emplace_back(time_point, "Valid : Time");
    }
    for (const auto& error_time : frame.received_error_times) {
        all_times.emplace_back(error_time, "Error : Time");
    }

    // Sort all times in ascending order
    std::sort(all_times.begin(), all_times.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    // Print sorted times with labels and matching payload sizes
    v_cout_2 << "Received Times and Payload Sizes in Order:" << frame.frame_number << "\n";
    for (size_t i = 0; i < all_times.size(); ++i) {

        auto formatted_time = formatTime(std::chrono::duration_cast<std::chrono::milliseconds>(all_times[i].first.time_since_epoch()));

        v_cout_2 << all_times[i].second << ": " << formatted_time;

        // Match with payload size if available
        if (i < frame.payload_sizes.size()) {
            v_cout_2 << ", Payload Size: " << frame.payload_sizes[i];
        }

        v_cout_2 << "\n";
    }
    v_cout_2 << "\n";

    if (!all_times.empty()) {
        auto first_time = all_times.front().first;
        auto last_time = all_times.back().first;
        auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(last_time - first_time).count();
        v_cout_2 << "Time Taken: " << time_diff << " ms" << "\n";
    }

    // Calculate total payload size
    size_t total_payload_size = std::accumulate(frame.payload_sizes.begin(), frame.payload_sizes.end(), size_t(0));
    v_cout_2 << "Total Size: " << total_payload_size << " bytes" << "\n";
    v_cout_2 << "\n" << std::endl;

#ifdef GUI_SET
    gui_window_number = 5;
#endif
}


void UVCPHeaderChecker::print_stats() const {
#ifdef TUI_SET
    window_number = 3;
    print_whole_flag = 1;
#elif GUI_SET
  gui_window_number = 4;
  print_whole_flag = 1;
#endif

    payload_stats.print_stats();
    frame_stats.print_stats();
    v_cout_1 << std::flush;

#ifdef TUI_SET
    print_whole_flag = 0;
    window_number = 1;
#elif GUI_SET
    print_whole_flag = 0;
  gui_window_number = 5;
#endif
}

void UVCPHeaderChecker::print_frame_data(const ValidFrame& frame) {
#ifdef GUI_SET
  if (frame.frame_error) {
    gui_window_number = 0;
  } else {
    gui_window_number = 13;
  }
#endif
    v_cout_2 << "Frame Number: " << frame.frame_number << "\n";
    v_cout_2 << "Toggle Bit (FID): " << static_cast<int>(frame.toggle_bit) << "\n";
    v_cout_2 << "Packet Number: " << frame.packet_number << "\n";
    v_cout_2 << "Frame PTS: " << frame.frame_pts << "\n";
    // Print Frame Error directly with switch statement
    v_cout_2 << "Frame Error: ";
    switch (frame.frame_error) {
        case ERR_FRAME_NO_ERROR:
            v_cout_2 << "No Error";
            break;
        case ERR_FRAME_DROP:
            v_cout_2 << "Frame Drop";
            break;
        case ERR_FRAME_ERROR:
            v_cout_2 << "Frame Error by Payload Header";
            break;
        case ERR_FRAME_MAX_FRAME_OVERFLOW:
            v_cout_2 << "Max Frame Overflow";
            break;
        case ERR_FRAME_INVALID_YUYV_RAW_SIZE:
            v_cout_2 << "Invalid YUYV Raw Size";
            break;
        case ERR_FRAME_SAME_DIFFERENT_PTS:
            v_cout_2 << "Same Different PTS";
            break;
        case ERR_FRAME_MISSING_EOF:
            v_cout_2 << "Missing EOF";
            break;
        default:
            v_cout_2 << "Unknown Error";
            break;
    }
    v_cout_2 << "\n";
    v_cout_2 << "EOF Reached: " << (frame.eof_reached ? "Yes" : "No") << "\n";

    // Calculate time taken from valid start to the last of error or valid times
    if (!frame.received_chrono_times.empty()) {
        auto valid_start = frame.received_chrono_times.front();
        auto valid_end = frame.received_chrono_times.back();
        auto error_end = !frame.received_error_times.empty() ? frame.received_error_times.back() : valid_end;

        // Choose the later of valid_end and error_end as the end point
        auto final_end = (error_end > valid_end) ? error_end : valid_end;
        auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(final_end - valid_start).count();

        auto valid_start_ms = formatTime(std::chrono::duration_cast<std::chrono::milliseconds>(valid_start.time_since_epoch()));
        auto final_end_ms = formatTime(std::chrono::duration_cast<std::chrono::milliseconds>(final_end.time_since_epoch()));

        v_cout_2 << "Time Taken: " << valid_start_ms << " ms ~ " << final_end_ms << " ms : " << time_taken << " ms" << "\n";
    } else {
        v_cout_2 << "No Valid Times Recorded" << "\n";
    }

    // Calculate total payload size
    size_t total_payload_size = std::accumulate(frame.payload_sizes.begin(), frame.payload_sizes.end(), size_t(0));
    v_cout_2 << "Total Payload Size: " << total_payload_size << " bytes" << "\n";

    v_cout_2 << std::endl;
#ifdef GUI_SET
  gui_window_number = 5;
#endif
}


void UVCPHeaderChecker::print_summary(const ValidFrame& frame) {
#ifdef GUI_SET
    print_whole_flag = 1;
    gui_window_number = 2;
#endif

    v_cout_2 << "Frame Number: " << frame.frame_number << "\n";

    if (!frame.received_chrono_times.empty()) {
      auto first_valid_time = frame.received_chrono_times.front();
      auto formatted_first_valid_time = formatTime(std::chrono::duration_cast<std::chrono::milliseconds>(first_valid_time.time_since_epoch()));
      v_cout_2 << "First Valid Time: " << formatted_first_valid_time << "\n";
    }

    v_cout_2 << "\nFrame Errors:" << "\n";

      v_cout_2 << " - Frame Error: " << frame.frame_error << "\n";
      printFrameErrorExplanation(frame.frame_error);
      size_t actual_frame_size = std::accumulate(frame.payload_sizes.begin(), frame.payload_sizes.end(), size_t(0));
      if (ControlConfig::get_frame_format() == "yuyv") {
        size_t expected_frame_size = ControlConfig::get_width() * ControlConfig::get_height() * 2;
        v_cout_2 << " - Frame Format: YUYV\n";
        v_cout_2 << "expected frame size: " << expected_frame_size << " bytes excluding the header length.\n";
        if (expected_frame_size != actual_frame_size) {
            std::ptrdiff_t diff = static_cast<std::ptrdiff_t>(actual_frame_size) - static_cast<std::ptrdiff_t>(expected_frame_size);
            v_cout_2 << "Data Loss :          " << diff << " bytes\n";
        }
      }
      v_cout_2 << "actual frame size:   " << actual_frame_size << "\n";

    v_cout_2 << "\nPayload Errors:" << "\n";

    if (frame.payload_errors.empty()) {
        v_cout_2 << "NO ERROR, NO DATA LOSS \n";
    } else {
        size_t temp_lost_data_size = 0;
        for (size_t i = 0; i < frame.payload_errors.size(); ++i) {
            v_cout_2 << " - Payload Error: " << frame.payload_errors[i] 
                    << ", Lost Data Size: " << frame.lost_data_sizes[i] << " bytes (includeing header) \n";

            printUVCErrorExplanation(frame.payload_errors[i]);

            temp_lost_data_size += frame.lost_data_sizes[i];
        }
        
        if (temp_lost_data_size > 0) {
            v_cout_2 << "Likely There is Data Loss in the Frame\n";
            v_cout_2 << "Total Lost Data Size: " << temp_lost_data_size << " bytes\n";
        }
    }

    // Calculate time taken from valid start to the last of error or valid times
    if (!frame.received_chrono_times.empty()) {
        auto valid_start = frame.received_chrono_times.front();
        auto valid_end = frame.received_chrono_times.back();
        auto error_end = !frame.received_error_times.empty() ? frame.received_error_times.back() : valid_end;

        // Choose the later of valid_end and error_end as the end point
        auto final_end = (error_end > valid_end) ? error_end : valid_end;
        auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(final_end - valid_start).count();

        if (time_taken > (1000.0 / (ControlConfig::fps)) + 20){
          v_cout_2 << "Frame Drop May Cause because of Time Taken (Valid Start to Last Event): \n"
          << "Should be " << (1000.0 / (ControlConfig::fps)) << " ms, but " << time_taken << " ms \n"
          << "Or two frames could be overlapped \n";
        }
    }
    v_cout_2 << "\n ---- \n";

// if current frame is out of boundary, likely data loss
// if error bit is set and data is present likely data loss
// if none of the case above, likely no data loss, valid
// if fid is same but pts is different, likely massive frame loss

// average frame size within boundary of 5% error 
  v_cout_2 << std::flush;

#ifdef GUI_SET
    print_whole_flag = 0;
    gui_window_number = 5;
#endif
}

void UVCPHeaderChecker::printUVCErrorExplanation(UVCError error) {

    if (error == ERR_NO_ERROR) {
        v_cout_2 << "No Error, Valid - No errors detected.\n";
    } else if (error == ERR_EMPTY_PAYLOAD) {
        v_cout_2 << "No Payload- UVC payload is empty.\nOccurs when there is no payload header.\n";
    } else if (error == ERR_MAX_PAYLAOD_OVERFLOW) {
        v_cout_2 << "Payload Overflow - UVC payload size exceeds max transfer size.\nOccurs if payload size is larger than the max payload set in the interface descriptor.\n";
    } else if (error == ERR_ERR_BIT_SET) {
        v_cout_2 << "BFH Error Bit Set - The Error bit in the UVC payload header is set.\n";
    } else if (error == ERR_LENGTH_OUT_OF_RANGE) {
        v_cout_2 << "Payload Header Length Out of Range - HLE is outside of expected range (2 to 12).\n";
    } else if (error == ERR_LENGTH_INVALID) {
        v_cout_2 << "Payload Header Length Incorrect with BFH - Header length does not match BFH flags.\nExpected values: PTS=0, SCR=0, HLE=2; PTS=1, SCR=1, HLE=6; PTS=0, SCR=1, HLE=8; PTS=1, SCR=1, HLE=12.\n";
    } else if (error == ERR_RESERVED_BIT_SET) {
        v_cout_2 << "BFH Reserved Bit Set - Reserved bit is set, only checked when EOF=0.\n";
    } else if (error == ERR_EOH_BIT) {
        v_cout_2 << "EOH Bit Error - EOH is not properly set.\n";
    } else if (error == ERR_TOGGLE_BIT_OVERLAPPED) {
        v_cout_2 << "Toggle Bit Frame Overlapped - Toggle Bit in BFH has overlapping error.\n";
    } else if (error == ERR_FID_MISMATCH) {
        v_cout_2 << "FID Mismatch - Frame Identifier mismatch with previous frame.\n";
    } else if (error == ERR_SWAP) {
        v_cout_2 << "BFH Toggle Bit Error with PTS Difference - PTS matches but Toggle Bit mismatch detected.\n";
    } else if (error == ERR_MISSING_EOF) {
        v_cout_2 << "Missing EOF - EOF expected but not found in payload header.\n";
    } else {
        v_cout_2 << "Unknown Error - The error code is not recognized.\n";
    }
  v_cout_2 << " \n";

}



void UVCPHeaderChecker::printFrameErrorExplanation(FrameError error) {

    if (error == ERR_FRAME_NO_ERROR) {
        v_cout_2 << "No Frame Error - No frame errors detected.\n";
    } else if (error == ERR_FRAME_DROP) {
        v_cout_2 << "Frame Drop - Frame rate is lower than expected.\nIndicates missing frames based on FPS measurement.\n";
    } else if (error == ERR_FRAME_ERROR) {
        v_cout_2 << "Frame Error - General frame error \nCaused by payload validation errors.\n";
    } else if (error == ERR_FRAME_MAX_FRAME_OVERFLOW) {
        v_cout_2 << "Max Frame Size Overflow - Frame size exceeds max frame size setting.\nIndicates potential dummy data or erroneous payload.\n";
        v_cout_2 << "Max Frame Size is " << ControlConfig::get_dwMaxVideoFrameSize() << " bytes.\n";
    } else if (error == ERR_FRAME_INVALID_YUYV_RAW_SIZE) {
        v_cout_2 << "YUYV Frame Length Error - YUYV frame length mismatch.\nExpected size for YUYV is width * height * 2.\n";
    } else if (error == ERR_FRAME_SAME_DIFFERENT_PTS) {
        v_cout_2 << "Same Frame Different PTS - Only PTS mismatch detected without other validation errors.\nPTS mismatch occurs without errors in toggle validation.\n";
    } else if (error == ERR_FRAME_MISSING_EOF) {
        v_cout_2 << "Missing EOF - EOF is not found in the frame.\n";
    } else {
        v_cout_2 << "Unknown Frame Error - The frame error code is not recognized.\n";
    }
  v_cout_2 << " \n";

}

std::string UVCPHeaderChecker::formatTime(std::chrono::milliseconds ms) {
    auto time_point = std::chrono::system_clock::time_point(ms);
    auto time_t_format = std::chrono::system_clock::to_time_t(time_point);

    struct tm time_info;
#ifdef _WIN32
    if (localtime_s(&time_info, &time_t_format) != 0 || time_info.tm_hour < 0 || time_info.tm_hour > 23) {
        return "00:00:00.000"; 
    }
#elif __linux__
    if (localtime_r(&time_t_format, &time_info) == nullptr || time_info.tm_hour < 0 || time_info.tm_hour > 23) {
        return "00:00:00.000"; 
    }
#endif
    auto milliseconds = ms.count() % 1000;

    std::ostringstream oss;
    oss << std::put_time(&time_info, "%H:%M:%S") << "." << std::setw(3) << std::setfill('0') << milliseconds;
    return oss.str();
}
