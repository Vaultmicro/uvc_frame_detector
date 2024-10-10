#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cstdint>
#include <utils/circular_buffer.hpp>

uint16_t combine_chars(char high, char low) {
    return (static_cast<uint16_t>(high) << 8) | static_cast<uint16_t>(low);
};

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

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::cout << "Start Pipe" << std::endl;
    std::cout.flush();

    std::string output_path = "C:\\Users\\gyuho\\win_p\\pipe.txt";
    
    std::ofstream log_file(output_path, std::ios::out | std::ios::app); 
    if (!log_file.is_open()) {
        std::cerr << "Error: Unable to open log file: " << output_path << std::endl;
        return 1;
    } else {
        std::cout << "Log file opened successfully: " << output_path << std::endl;
    }

    CircularBuffer <char> data_circular_buffer(131072*30);
    CircularBuffer <double> time_circular_buffer(30);
    CircularBuffer <int> len_circular_buffer(30);

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

        // iso 0x00 int 0x01 control 0x02 bulk 0x03
        // if usb_transfer_type is 0x00 iso devide usb_capdata by ,

        if (usb_transfer_type == "0x00") {
            std::vector<std::string> capdata_tokens = split(usb_capdata, ',');
            for (const auto& token : capdata_tokens) {
                // Store each character in the circular buffer
                data_circular_buffer.enqueue(token.c_str(), token.size());
                double frame_time_epoch_double = std::stod(frame_time_epoch);
                time_circular_buffer.enqueue(&frame_time_epoch_double, 1); // Enqueue the frame time epoch as double
                int frame_len_int = std::stoi(frame_len);
                len_circular_buffer.enqueue(&frame_len_int, 1);         // Enqueue the frame length as int
            }
        } else if (usb_transfer_type == "0x01"){
            //skip
        } else if (usb_transfer_type == "0x02"){
            //skip
        } else if (usb_transfer_type == "0x03"){
            data_circular_buffer.enqueue(usb_capdata.c_str(), usb_capdata.size());
            double frame_time_epoch_double = std::stod(frame_time_epoch);
            time_circular_buffer.enqueue(&frame_time_epoch_double, 1); // Enqueue the frame time epoch as double
            int frame_len_int = std::stoi(frame_len);
            len_circular_buffer.enqueue(&frame_len_int, 1);         // Enqueue the frame length as int
        } else{
            //error
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

    return 0;
}
