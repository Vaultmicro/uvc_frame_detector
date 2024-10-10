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

#include <utils/circular_buffer.hpp>
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
//   {
//     std::lock_guard<std::mutex> lock(queue_mutex);
//     stop_processing = true;
//   }
//   queue_cv.notify_all();

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

void capture_packets(CircularBuffer<char>& data_circular_buffer, 
                     CircularBuffer<double>& time_circular_buffer, 
                     CircularBuffer<int>& len_circular_buffer) {
    std::string output_path = "C:\\Users\\gyuho\\win_p\\pipe.txt";
    std::ofstream log_file(output_path, std::ios::out | std::ios::app); 

    if (!log_file.is_open()) {
        std::cerr << "Error: Unable to open log file: " << output_path << std::endl;
        return;
    } else {
        std::cout << "Log file opened successfully: " << output_path << std::endl;
    }

    std::string line;
    std::cout << "Waiting for input..." << std::endl;

    while (std::getline(std::cin, line)) {
        // Split the line by semicolon
        std::vector<std::string> tokens = split(line, ';');

        // Prepare fields with defaults if they are missing
        std::string usb_transfer_type = tokens.size() > 0 ? tokens[0] : "N/A";
        std::string frame_time_epoch = tokens.size() > 1 ? tokens[1] : "N/A";
        std::string frame_len = tokens.size() > 2 ? tokens[2] : "N/A";
        std::string usb_capdata = tokens.size() > 3 ? tokens[3] : "N/A";

        // Process based on usb_transfer_type
        if (usb_transfer_type == "0x00") {
            std::vector<std::string> capdata_tokens = split(usb_capdata, ',');
            for (const auto& token : capdata_tokens) {
                data_circular_buffer.enqueue(token.c_str(), token.size());
                double frame_time_epoch_double = std::stod(frame_time_epoch);
                time_circular_buffer.enqueue(&frame_time_epoch_double, 1);
                int frame_len_int = std::stoi(frame_len);
                len_circular_buffer.enqueue(&frame_len_int, 1);
            }
        } else if (usb_transfer_type == "0x01") {
            // Skip interrupt transfer
        } else if (usb_transfer_type == "0x02") {
            // Skip control transfer
        } else if (usb_transfer_type == "0x03") {
            data_circular_buffer.enqueue(usb_capdata.c_str(), usb_capdata.size());
            double frame_time_epoch_double = std::stod(frame_time_epoch);
            time_circular_buffer.enqueue(&frame_time_epoch_double, 1);
            int frame_len_int = std::stoi(frame_len);
            len_circular_buffer.enqueue(&frame_len_int, 1);
        } else {
            // Handle unexpected transfer type
        }

        // Print each field separately
        // std::cout << "frame.time_epoch: " << frame_time_epoch << std::endl;
        // std::cout << "frame.len: " << frame_len << std::endl;
        // std::cout << "usb.capdata: " << usb_capdata << std::endl;

        // Log the separated fields
        log_file << "frame.time_epoch: " << frame_time_epoch << std::endl;
        log_file << "frame.len: " << frame_len << std::endl;
        log_file << "usb.capdata: " << usb_capdata << std::endl;

        log_file.flush();
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
    CircularBuffer<char> data_circular_buffer(131072 * 30);
    CircularBuffer<double> time_circular_buffer(30);
    CircularBuffer<int> len_circular_buffer(30);

    // Create threads for capture and processing
    std::thread capture_thread(capture_packets, 
                               std::ref(data_circular_buffer), 
                               std::ref(time_circular_buffer), 
                               std::ref(len_circular_buffer));

    std::thread process_thread(process_packets);  // Process packets in another thread (empty for now)

    // Wait for the threads to finish
    capture_thread.join();
    process_thread.join();

    clean_exit(0);
    std::cout << "End of main" << std::endl;
    return 0;
}
