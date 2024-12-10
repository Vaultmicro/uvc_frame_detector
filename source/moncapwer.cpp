/*********************************************************************
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

#include "moncapwer.hpp"

std::queue<std::chrono::time_point<std::chrono::steady_clock>> time_records;
std::mutex time_mutex;
std::queue<std::vector<u_char>> packet_queue;
std::mutex queue_mutex;
std::condition_variable queue_cv;
bool stop_processing = false;
std::queue<std::tuple<int, int, int, std::string, uint32_t, uint32_t, uint32_t, std::chrono::time_point<std::chrono::steady_clock>>> set_control_queue;


struct FrameInfo{
  int frame_width;
  int frame_height;
  int frame_format_subtype;
};

void clean_exit(int signum) {
  {
    std::lock_guard<std::mutex> lock(queue_mutex);
    stop_processing = true;
  }
  queue_cv.notify_all();

//   if (log_file.is_open()) {
//     log_file.close();
//   }

  CtrlPrint::v_cout_2 << "Exiting safely..." << std::endl;
  std::cout << "End of the process: wait for other pipes to be closed" << std::endl;

  exit(signum);
}

// Helper function to split the input line by a delimiter
std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = s.find(delimiter);

    while (end != std::string::npos) {
        tokens.emplace_back(s.substr(start, end - start));
        start = end + 1;
        end = s.find(delimiter, start);
    }
    tokens.emplace_back(s.substr(start));
    return tokens;
}

std::chrono::time_point<std::chrono::steady_clock> convert_epoch_to_time_point(double frame_time_epoch) {
    auto duration = std::chrono::duration<double>(frame_time_epoch);
    auto time_point = std::chrono::steady_clock::time_point(std::chrono::duration_cast<std::chrono::steady_clock::duration>(duration));
    return time_point;
}


const std::array<u_char, 256> hex_lut = []() {
    std::array<u_char, 256> table = {};
    for (int i = 0; i < 256; ++i) table[i] = 0xFF;
    for (char c = '0'; c <= '9'; ++c) table[static_cast<unsigned char>(c)] = c - '0';
    for (char c = 'A'; c <= 'F'; ++c) table[static_cast<unsigned char>(c)] = c - 'A' + 10;
    for (char c = 'a'; c <= 'f'; ++c) table[static_cast<unsigned char>(c)] = c - 'a' + 10;
    return table;
}();


void hex_string_to_bytes_append(const std::string& hex_str, std::vector<u_char>& out_vec) {
    size_t len = hex_str.length();
    size_t num_bytes = len / 2;
    size_t initial_size = out_vec.size();
    out_vec.resize(initial_size + num_bytes);

    const char* src = hex_str.data();
    u_char* dst = out_vec.data() + initial_size;

    for (size_t i = 0; i < num_bytes; ++i) {
        u_char high = hex_lut[static_cast<unsigned char>(src[i * 2])];
        u_char low = hex_lut[static_cast<unsigned char>(src[i * 2 + 1])];
        dst[i] = (high << 4) | low;
    }
}


void capture_packets() {

    static std::vector<u_char> temp_buffer;
    static uint32_t bulk_maxlengthsize = 0;

    std::string line;
#ifdef GUI_SET
    gui_window_number = 5;
    CtrlPrint::v_cout_1 << "Waiting for input...     " << std::endl;
#else
    CtrlPrint::v_cout_1 << "Waiting for input...     " << std::endl;
#endif
    while (std::getline(std::cin, line)) {
        // Split the line by semicolon
        std::vector<std::string> tokens = split(line, ';');

        // Prepare fields with defaults if they are missing
        // -e usb.transfer_type -e frame.time_epoch -e frame.len -e usb.capdata or usb.iso.data
        // MUST BE IN CORRECT ORDER
        std::string usb_transfer_type = (tokens.size() > 0 && !tokens[0].empty()) ? tokens[0] : "N/A";
        std::string frame_time_epoch = (tokens.size() > 1 && !tokens[1].empty()) ? tokens[1] : "N/A";
        std::string frame_len = (tokens.size() > 2 && !tokens[2].empty()) ? tokens[2] : "N/A";
        std::string usb_capdata = (tokens.size() > 3 && !tokens[3].empty()) ? tokens[3] : "N/A";
        std::string usb_isodata = (tokens.size() > 4 && !tokens[4].empty()) ? tokens[4] : "N/A";
        std::string format_index = (tokens.size() > 5 && !tokens[5].empty()) ? tokens[5] : "N/A";
        std::string frame_index = (tokens.size() > 6 && !tokens[6].empty()) ? tokens[6] : "N/A";
        std::string frame_widths = (tokens.size() > 7 && !tokens[7].empty()) ? tokens[7] : "N/A";
        std::string frame_heights = (tokens.size() > 8 && !tokens[8].empty()) ? tokens[8] : "N/A";
        std::string subtype_frame_format = (tokens.size() > 9 && !tokens[9].empty()) ? tokens[9] : "N/A";
        std::string frame_interval_fps = (tokens.size() > 10 && !tokens[10].empty()) ? tokens[10] : "N/A";
        std::string max_frame_size = (tokens.size() > 11 && !tokens[11].empty()) ? tokens[11] : "N/A";
        std::string max_payload_size = (tokens.size() > 12 && !tokens[12].empty()) ? tokens[12] : "N/A";
        std::string num_frame_descriptor = (tokens.size() > 13 && !tokens[13].empty()) ? tokens[13] : "N/A";
        std::string time_frequency = (tokens.size() > 14 && !tokens[14].empty()) ? tokens[14] : "N/A";

        auto time_point_d = (frame_time_epoch != "N/A") ? convert_epoch_to_time_point(std::stod(frame_time_epoch)) : std::chrono::steady_clock::time_point{};

        uint32_t frame_length = (frame_len != "N/A") ? std::stoul(frame_len) : 0;

#ifdef __linux__
        // This for linux urb
        static int start_flag = 0;
#endif

        if (usb_capdata == "N/A" && usb_isodata == "N/A" && format_index == "N/A") {
            continue;
        } else {

          // Process based on usb_transfer_type
          if (usb_transfer_type == "0x00") {
              std::vector<std::string> capdata_tokens = split(usb_isodata, ',');

              for (const std::string& token : capdata_tokens) {
                  std::vector<u_char> temp_buffer;
                  temp_buffer.reserve(token.length() / 2);

                  hex_string_to_bytes_append(token, temp_buffer);

                  {
                      std::lock_guard<std::mutex> lock(queue_mutex);
                      packet_queue.push(std::move(temp_buffer));

                      std::lock_guard<std::mutex> time_lock(time_mutex);
                      time_records.push(std::move(time_point_d));
                  }

                  queue_cv.notify_one();
              }

          } else if (usb_transfer_type == "0x01") {
              // Skip interrupt transfer
          } else if (usb_transfer_type == "0x02") {

            std::vector<std::string> format_indices = split(format_index, ',');
            std::vector<std::string> frame_indices = split(frame_index, ',');
            std::vector<std::string> frame_widths_list = split(frame_widths, ',');
            std::vector<std::string> frame_heights_list = split(frame_heights, ',');
            std::vector<std::string> frame_formats = split(subtype_frame_format, ',');
            std::vector<std::string> num_frame_descriptor_list = split(num_frame_descriptor, ',');
             
            static std::map<int, std::map<int, FrameInfo>> format_map;
            static uint32_t time_frequency_ = 0;

            if (frame_widths != "N/A" && frame_heights != "N/A") {

              // std::cout << "in" << std::endl;

              size_t format_index_counter = 0;
              int count = 0;

              std::vector<std::string> filtered_subtype_frame_format;
              for (const auto& value : frame_formats) {
                  int num_value = std::stoi(value);
                  // Exclude 1, 4, 6, 12, 13, 16
                  if (num_value != 1 && num_value != 4 && num_value != 6 && num_value != 12 && num_value != 13 &&  num_value != 16) {
                      filtered_subtype_frame_format.push_back(value);
                  }
              }

              for (size_t i = 0; i < frame_indices.size(); ++i) {
                if (count >= std::stoi(num_frame_descriptor_list[format_index_counter])) {
                    ++format_index_counter;
                    count = 0;
                }

                FrameInfo frame_info;
                frame_info.frame_width = std::stoi(frame_widths_list[i]);
                frame_info.frame_height = std::stoi(frame_heights_list[i]);
                frame_info.frame_format_subtype = std::stoi(filtered_subtype_frame_format[i]);

                // Insert into map where the key is format_index value, and frame_index value maps to FrameInfo
                int format_key = std::stoi(format_indices[format_index_counter]);
                int frame_key = std::stoi(frame_indices[i]);

                format_map[format_key][frame_key] = frame_info;

                count ++;
              }

              time_frequency_ = (time_frequency != "N/A") ? std::stol(time_frequency) : 0;

            }

            if (max_frame_size != "N/A" && max_payload_size != "N/A") {
              
              std::vector<std::string> format_indices = split(format_index, ',');
              std::vector<std::string> frame_indices = split(frame_index, ',');

              int format_index_int = std::stoi(format_indices[0]);
              int frame_index_int = std::stoi(frame_indices[0]);

              // std::cout << "format_index_int: " << format_index_int << std::endl;
              // std::cout << "frame_index_int: " << frame_index_int << std::endl;

              if (format_map.find(format_index_int) != format_map.end() &&
                  format_map[format_index_int].find(frame_index_int) != format_map[format_index_int].end()) {

                  int width = format_map[format_index_int][frame_index_int].frame_width;
                  int height = format_map[format_index_int][frame_index_int].frame_height;
                  int frame_format_subtype = format_map[format_index_int][frame_index_int].frame_format_subtype;
              
                  std::string frame_format;
                  switch (frame_format_subtype) {
                      case 5:
                      //uncompressed
                          frame_format = "yuyv";
                          break;
                      case 7:
                      //mjpeg
                          frame_format = "mjpeg";
                          break;
                    //   case 13:
                    //   //color format
                    //       frame_format = "rgb";
                    //       break;
                      case 17:
                      //frame based
                          frame_format = "h264";
                          break;
                      default:
                          std::cerr << "Unsupported frame format subtype: " << frame_format_subtype << ". Using default format 'mjpeg'." << std::endl;
                          frame_format = "mjpeg";
                          break;
                  }

                    std::tuple<int, int, int, std::string, uint32_t, uint32_t, uint32_t, std::chrono::time_point<std::chrono::steady_clock>> control_data = 
                        std::make_tuple(width, height, 10000000 / std::stoi(frame_interval_fps), frame_format, std::stoi(max_frame_size), std::stoi(max_payload_size), 
                        time_frequency_, time_point_d);
                  {
                      std::lock_guard<std::mutex> lock(queue_mutex);
                      set_control_queue.push(std::move(control_data));
                  }
                  queue_cv.notify_one();

              } else {
                  std::cerr << "Error: Invalid format_index or frame_index." << std::endl;
              }

            }

          } else if (usb_transfer_type == "0x03") {
#ifdef __linux__
    if (bulk_maxlengthsize == frame_length && start_flag == 1) {
        hex_string_to_bytes_append(usb_capdata, temp_buffer);
    } else {
        if (bulk_maxlengthsize < frame_length) {
            bulk_maxlengthsize = frame_length;
        }
        start_flag = 1;
        temp_buffer.clear();
        temp_buffer.reserve(frame_length / 2);
        hex_string_to_bytes_append(usb_capdata, temp_buffer);
    }
#else
    temp_buffer.clear();
    temp_buffer.reserve(frame_length / 2);
    hex_string_to_bytes_append(usb_capdata, temp_buffer);
#endif
              {
                  std::lock_guard<std::mutex> lock(queue_mutex);
                  packet_queue.push(std::move(temp_buffer));

                  std::lock_guard<std::mutex> time_lock(time_mutex);
                  time_records.push(std::move(time_point_d));
              }
              temp_buffer.clear();
              queue_cv.notify_one();


          } else {
              // Handle unexpected transfer type
          }

        }

        // // Print each field separately
        // CtrlPrint::v_cout_1 << "frame.time_epoch: " << frame_time_epoch << std::endl;
        // CtrlPrint::v_cout_1 << "frame.len: " << frame_len << std::endl;
        // CtrlPrint::v_cout_1 << "usb.capdata: " << usb_capdata << std::endl;

        // // Log the separated fields
        // log_file << "usb_transfer_type: " << usb_transfer_type << std::endl;
        // log_file << "frame.time_epoch: " << frame_time_epoch << std::endl;
        // log_file << "frame.len: " << frame_len << std::endl;
        // log_file << "usb.capdata: " << usb_capdata << std::endl;

        // log_file.flush();
    }

    // log_file.close();
    // CtrlPrint::v_cout_1 << "Log file closed." << std::endl;

}


void process_packets() {
  UVCPHeaderChecker header_checker;

  while (true) {
    std::unique_lock<std::mutex> lock(queue_mutex);

    queue_cv.wait(lock,
                  [] { return !packet_queue.empty() || !set_control_queue.empty() || stop_processing; });

    if (stop_processing && packet_queue.empty() && set_control_queue.empty()) {
      break;
    }
    
    if (!set_control_queue.empty()){
        auto control_data = std::move(set_control_queue.front());
        set_control_queue.pop();
        lock.unlock();

        int width, height, fps;
        std::string frame_format;
        uint64_t max_frame_size, max_payload_size, time_frequency;
        std::chrono::time_point<std::chrono::steady_clock> received_time;
        std::tie(width, height, fps, frame_format, max_frame_size, max_payload_size, time_frequency, received_time) = control_data;

        CtrlPrint::v_cout_3 << "Processing control configuration" << std::endl;
        header_checker.control_configuration_ctrl(width, height, fps, frame_format, max_frame_size, max_payload_size, time_frequency, received_time);

    }else if (!packet_queue.empty()) {

      auto packet = std::move(packet_queue.front());
      packet_queue.pop();
      lock.unlock();

      std::chrono::time_point<std::chrono::steady_clock> received_time;
      {
        std::lock_guard<std::mutex> time_lock(time_mutex);
        received_time = time_records.front();
        time_records.pop();
      }

      if (!packet.empty()) {
        CtrlPrint::v_cout_3 << "Processing packet of size: " << packet.size() << std::endl;
      }

      uint8_t valid_err =
          header_checker.payload_valid_ctrl(packet, received_time);

      if (valid_err) {
        CtrlPrint::v_cerr_3 << "Invalid packet detected" << std::endl;
        continue;
      }
    } else {
      lock.unlock();
    }
  }
  CtrlPrint::v_cout_1 << "Process packet() end" << std::endl;
}




void develope_frame_image() {
    while (true){
        std::unique_lock<std::mutex> lock(dev_f_image_mutex);
        dev_f_image_cv.wait(lock, [] { return !dev_f_image_queue.empty(); });
        
        auto frame_format = std::move(dev_f_image_format_queue.front());
        dev_f_image_format_queue.pop();
        auto frame_data = std::move(dev_f_image_queue.front());
        dev_f_image_queue.pop();

        lock.unlock();

        develope_photo(frame_format, frame_data);
    }
}


int main(int argc, char* argv[]) {

    bool fw_set = false;
    bool fh_set = false;
    bool fps_set = false;
    bool ff_set = false;

    for (int i = 1; i < argc; i += 2) {
        if (std::strcmp(argv[i], "-fw") == 0 && i + 1 < argc) {
        ControlConfig::instance().set_width(std::atoi(argv[i + 1]));
        fw_set = true;
        } else if (std::strcmp(argv[i], "-fh") == 0 && i + 1 < argc) {
        ControlConfig::instance().set_height(std::atoi(argv[i + 1]));
        fh_set = true;
        } else if (std::strcmp(argv[i], "-fps") == 0 && i + 1 < argc) {
        ControlConfig::instance().set_fps(std::atoi(argv[i + 1]));
        fps_set = true;
        } else if (std::strcmp(argv[i], "-ff") == 0 && i + 1 < argc) {
        ControlConfig::instance().set_frame_format(argv[i + 1]);
        ff_set = true;
        } else if (std::strcmp(argv[i], "-mf") == 0 && i + 1 < argc) {
        ControlConfig::instance().set_dwMaxVideoFrameSize(std::atoi(argv[i + 1]));
        } else if (std::strcmp(argv[i], "-mp") == 0 && i + 1 < argc) {
        ControlConfig::instance().set_dwMaxPayloadTransferSize(std::atoi(argv[i + 1]));
        } else if (std::strcmp(argv[i], "-v") == 0 && i + 1 < argc) {
        VerboseStream::verbose_level = std::atoi(argv[i + 1]);
        } else {
        CtrlPrint::v_cerr_1 << "Usage: " << argv[0]
                <<  "[-fw frame_width] [-fh frame_height] [-fps frame_per_sec] "
                    "[-ff frame_format] [-mf max_frame_size] [-mp max_payload_size] "
                    "[-v verbose_level]"
                << std::endl;
        return 1;
        }
    }
#ifdef GUI_SET
  temp_window_number = gui_window_number;
  gui_window_number = 3;
#endif
    if (!fw_set || !fh_set || !fps_set || !ff_set) {
        if (!fw_set) {
        CtrlPrint::v_cout_1 << "Frame width not specified, using default: "
                    << ControlConfig::instance().get_width() << std::endl;
        }
        if (!fh_set) {
        CtrlPrint::v_cout_1 << "Frame height not specified, using default: "
                    << ControlConfig::instance().get_height() << std::endl;
        }
        if (!fps_set) {
        CtrlPrint::v_cout_1 << "FPS not specified, using default: "
                    << ControlConfig::instance().get_fps() << std::endl;
        }
        if (!ff_set) {
        CtrlPrint::v_cout_1 << "Frame format not specified, using default: "
                    << ControlConfig::instance().get_frame_format() << std::endl;
        }
    }

#ifdef GUI_SET
    gui_window_number = temp_window_number;
#else
    CtrlPrint::v_cout_1 << "Frame Width: " << ControlConfig::instance().get_width() << std::endl;
    CtrlPrint::v_cout_1 << "Frame Height: " << ControlConfig::instance().get_height() << std::endl;
    CtrlPrint::v_cout_1 << "Frame FPS: " << ControlConfig::instance().get_fps() << std::endl;
    CtrlPrint::v_cout_1 << "Frame Format: " << ControlConfig::instance().get_frame_format()
            << std::endl;
#endif

    std::signal(SIGINT, clean_exit);
    std::signal(SIGTERM, clean_exit);

    // Create threads for capture and processing
    std::thread capture_thread(capture_packets);

    std::thread process_thread(process_packets);

    std::thread fdevelope_thread(develope_frame_image);

#ifdef GUI_SET
    if (start_screen() == -1) {
        return -1;
    }
    screen();
#endif

    // Wait for the threads to finish
    capture_thread.join();
    process_thread.join();
    fdevelope_thread.join();
    
    clean_exit(0);
#ifdef GUI_SET
  end_screen();
#else
    CtrlPrint::v_cout_1 << "End of main" << std::endl;
#endif


    return 0;
}
