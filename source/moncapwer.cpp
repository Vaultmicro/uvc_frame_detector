#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cstdint>
#include <thread>
#include <mutex>
#include <queue>
#include <csignal>

#include <validuvc/control_config.hpp>
#include <validuvc/uvcpheader_checker.hpp>
#include <utils/verbose.hpp>

std::queue<std::chrono::time_point<std::chrono::steady_clock>> time_records;
std::mutex time_mutex;
std::queue<std::vector<u_char>> packet_queue;
std::mutex queue_mutex;
std::condition_variable queue_cv;
bool stop_processing = false;


void clean_exit(int signum) {
  {
    std::lock_guard<std::mutex> lock(queue_mutex);
    stop_processing = true;
  }
  queue_cv.notify_all();

//   if (log_file.is_open()) {
//     log_file.close();
//   }

  v_cout_2 << "Exiting safely..." << std::endl;

  //exit(signum);
}


// Helper function to split the input line by a delimiter
std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::chrono::time_point<std::chrono::steady_clock> convert_epoch_to_time_point(double frame_time_epoch) {
    auto duration = std::chrono::duration<double>(frame_time_epoch);
    auto time_point = std::chrono::steady_clock::time_point(std::chrono::duration_cast<std::chrono::steady_clock::duration>(duration));
    return time_point;
}

std::vector<u_char> hex_string_to_bytes(const std::string& hex) {
    std::vector<u_char> bytes;
    try {
        for (size_t i = 0; i < hex.length(); i += 2) {
            std::string byte_string = hex.substr(i, 2); 
            u_char byte = static_cast<u_char>(std::stoul(byte_string, nullptr, 16));
            bytes.push_back(byte);
        }
    } catch (const std::invalid_argument&) {
        std::cerr << "Invalid argument error." << std::endl;
    } catch (const std::out_of_range&) {
        std::cerr << "Out of range error." << std::endl;
    }
    return bytes;
}



void capture_packets() {
    std::string output_path = "C:\\Users\\gyuho\\uvc_frame_detector\\log\\pipe.txt";
    std::ofstream log_file(output_path, std::ios::out | std::ios::app); 

    if (!log_file.is_open()) {
        std::cerr << "Error: Unable to open log file: " << output_path << std::endl;
        return;
    } else {
        std::cout << "Log file opened successfully: " << output_path << std::endl;
    }

    static std::vector<u_char> temp_buffer;
    static uint32_t bulk_maxlengthsize = 0;

    std::string line;
    std::cout << "Waiting for input..." << std::endl;

    while (std::getline(std::cin, line)) {
        // Split the line by semicolon
        std::vector<std::string> tokens = split(line, ';');

        // Prepare fields with defaults if they are missing
        // -e usb.transfer_type -e frame.time_epoch -e frame.len -e usb.capdata or usb.iso.data
        // MUST BE IN CORRECT ORDER
        std::string usb_transfer_type = tokens.size() > 0 ? tokens[0] : "N/A";
        std::string frame_time_epoch = tokens.size() > 1 ? tokens[1] : "N/A";
        std::string frame_len = tokens.size() > 2 ? tokens[2] : "N/A";
        std::string usb_capdata = tokens.size() > 3 ? tokens[3] : "N/A";
        auto time_point = (frame_time_epoch != "N/A") ? convert_epoch_to_time_point(std::stod(frame_time_epoch)) : std::chrono::steady_clock::time_point{};

        uint32_t frame_length = (frame_len != "N/A") ? std::stoul(frame_len) : 0;
        static int start_flag = 0;

        if (usb_capdata == "N/A") {
            continue;
        } else {

          // Process based on usb_transfer_type
          if (usb_transfer_type == "0x00") {
            std::vector<std::string> capdata_tokens = split(usb_capdata, ',');
            
            for (const std::string& token : capdata_tokens) {
                temp_buffer = hex_string_to_bytes(token);

                log_file << "temp_buffer: ";
                for (u_char byte : temp_buffer) {
                    log_file << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
                }

                {
                    std::lock_guard<std::mutex> lock(queue_mutex);
                    packet_queue.push(temp_buffer);

                    std::lock_guard<std::mutex> time_lock(time_mutex);
                    time_records.push(time_point);
                }
                temp_buffer.clear();
                queue_cv.notify_one();
            }
          } else if (usb_transfer_type == "0x01") {
              // Skip interrupt transfer
          } else if (usb_transfer_type == "0x02") {
              // Skip control transfer
          } else if (usb_transfer_type == "0x03") {

            if (bulk_maxlengthsize == frame_length && start_flag == 1) {
              // Continue the transfer
              std::vector<u_char> new_data = hex_string_to_bytes(usb_capdata);
              temp_buffer.insert(temp_buffer.end(), new_data.begin(), new_data.end());
              // temp_buffer = hex_string_to_bytes(usb_capdata);
            } else {
              if (bulk_maxlengthsize < frame_length) {
                bulk_maxlengthsize = frame_length;
              }

              start_flag = 1;
              std::vector<u_char> new_data = hex_string_to_bytes(usb_capdata);
              temp_buffer.insert(temp_buffer.end(), new_data.begin(), new_data.end());
              // temp_buffer = hex_string_to_bytes(usb_capdata);

              // log_file << "temp_buffer: ";
              // for (u_char byte : temp_buffer) {
              //     log_file << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
              // }

              {
                  std::lock_guard<std::mutex> lock(queue_mutex);
                  packet_queue.push(temp_buffer);

                  std::lock_guard<std::mutex> time_lock(time_mutex);
                  time_records.push(time_point);
              }
              temp_buffer.clear();
              queue_cv.notify_one();
            }

          } else {
              // Handle unexpected transfer type
          }

        }

        // // Print each field separately
        // std::cout << "frame.time_epoch: " << frame_time_epoch << std::endl;
        // std::cout << "frame.len: " << frame_len << std::endl;
        // std::cout << "usb.capdata: " << usb_capdata << std::endl;

        // // Log the separated fields
        // log_file << "usb_transfer_type: " << usb_transfer_type << std::endl;
        // log_file << "frame.time_epoch: " << frame_time_epoch << std::endl;
        // log_file << "frame.len: " << frame_len << std::endl;
        // log_file << "usb.capdata: " << usb_capdata << std::endl;

        // log_file.flush();
    }

    log_file.close();
    std::cout << "Log file closed." << std::endl;
}


void process_packets() {
  UVCPHeaderChecker header_checker;

  while (true) {
    std::unique_lock<std::mutex> lock(queue_mutex);

    queue_cv.wait(lock,
                  [] { return !packet_queue.empty() || stop_processing; });

    if (stop_processing && packet_queue.empty()) {
      break;
    }
    
    if (!packet_queue.empty()) {

      auto packet = packet_queue.front();
      packet_queue.pop();
      lock.unlock();

      std::chrono::time_point<std::chrono::steady_clock> received_time;
      {
        std::lock_guard<std::mutex> time_lock(time_mutex);
        received_time = time_records.front();
        time_records.pop();
      }

      if (!packet.empty()) {
        v_cout_3 << "Processing packet of size: " << packet.size() << std::endl;
      }

      uint8_t valid_err =
          header_checker.payload_valid_ctrl(packet, received_time);

      if (valid_err) {
        v_cerr_3 << "Invalid packet detected" << std::endl;
        continue;
      }
    } else {
      lock.unlock();
    }
    // header_checker.print_packet(packet);
  }
  std::cout << "Process packet() end" << std::endl;
}


int main(int argc, char* argv[]) {

    bool fw_set = false;
    bool fh_set = false;
    bool fps_set = false;
    bool ff_set = false;

    for (int i = 1; i < argc; i += 2) {
        if (std::strcmp(argv[i], "-fw") == 0 && i + 1 < argc) {
        ControlConfig::set_width(std::atoi(argv[i + 1]));
        fw_set = true;
        } else if (std::strcmp(argv[i], "-fh") == 0 && i + 1 < argc) {
        ControlConfig::set_height(std::atoi(argv[i + 1]));
        fh_set = true;
        } else if (std::strcmp(argv[i], "-fps") == 0 && i + 1 < argc) {
        ControlConfig::set_fps(std::atoi(argv[i + 1]));
        fps_set = true;
        } else if (std::strcmp(argv[i], "-ff") == 0 && i + 1 < argc) {
        ControlConfig::set_frame_format(argv[i + 1]);
        ff_set = true;
        } else if (std::strcmp(argv[i], "-mf") == 0 && i + 1 < argc) {
        ControlConfig::set_dwMaxVideoFrameSize(std::atoi(argv[i + 1]));
        } else if (std::strcmp(argv[i], "-mp") == 0 && i + 1 < argc) {
        ControlConfig::set_dwMaxPayloadTransferSize(std::atoi(argv[i + 1]));
        } else if (std::strcmp(argv[i], "-v") == 0 && i + 1 < argc) {
        verbose_level = std::atoi(argv[i + 1]);
        } else {
        v_cerr_1 << "Usage: " << argv[0]
                <<  "[-fw frame_width] [-fh frame_height] [-fps frame_per_sec] "
                    "[-ff frame_format] [-mf max_frame_size] [-mp max_payload_size] "
                    "[-v verbose_level]"
                << std::endl;
        return 1;
        }
    }
    if (!fw_set || !fh_set || !fps_set || !ff_set) {
        if (!fw_set) {
        std::cout << "Frame width not specified, using default: "
                    << ControlConfig::get_width() << std::endl;
        }
        if (!fh_set) {
        std::cout << "Frame height not specified, using default: "
                    << ControlConfig::get_height() << std::endl;
        }
        if (!fps_set) {
        std::cout << "FPS not specified, using default: "
                    << ControlConfig::get_fps() << std::endl;
        }
        if (!ff_set) {
        std::cout << "Frame format not specified, using default: "
                    << ControlConfig::get_frame_format() << std::endl;
        }
    }
    v_cout_1 << "Frame Width: " << ControlConfig::get_width() << std::endl;
    v_cout_1 << "Frame Height: " << ControlConfig::get_height() << std::endl;
    v_cout_1 << "Frame FPS: " << ControlConfig::get_fps() << std::endl;
    v_cout_1 << "Frame Format: " << ControlConfig::get_frame_format()
            << std::endl;

    std::signal(SIGINT, clean_exit);
    std::signal(SIGTERM, clean_exit);


    // Create buffers

    // Create threads for capture and processing
    std::thread capture_thread(capture_packets);

    std::thread process_thread(process_packets);  // Process packets in another thread (empty for now)

    // Wait for the threads to finish
    capture_thread.join();
    process_thread.join();

    clean_exit(0);
    std::cout << "End of main" << std::endl;
    return 0;
}
