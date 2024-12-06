#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <string.h>
#include "validuvc/uvcpheader_checker.hpp"

#ifdef TUI_SET
#include "utils/tui_win.hpp"
#elif GUI_SET
#include "gui/gui_win.hpp"
#endif
#include "validuvc/control_config.hpp"

void process_file(UVCPHeaderChecker& whole_frame_checker, const std::string& filename) {
    std::ifstream inputFile(filename);
    std::string line;

    if (!inputFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();  // Default current time

    while (std::getline(inputFile, line)) {
        std::vector<u_char> packet;

        // If the line starts with "Converted Monotonic Time:", set the current_time
        if (line.find("Converted Monotonic Time:") == 0) {
            std::string time_string = line.substr(strlen("Converted Monotonic Time: "));
            double monotonic_seconds = std::stod(time_string);
            current_time = std::chrono::steady_clock::time_point(std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(monotonic_seconds)));
            continue;
        }

        // Convert line from hex string to packet data
        for (size_t i = 0; i < line.length(); i += 2) {
            std::string byteString = line.substr(i, 2);
            u_char byte = static_cast<u_char>(std::stoi(byteString, nullptr, 16));
            packet.push_back(byte);
        }

        // Use the current_time (either the default or the extracted one)
        uint8_t valid_err = whole_frame_checker.payload_valid_ctrl(packet, current_time);
    }

    inputFile.close();
}


int main() {
    UVCPHeaderChecker whole_frame_checker;
    // std::ifstream inputFile("../tests/change_shark/brio_new.txt"); 
    const std::string filename = "../tests/change_shark/brio_new.txt";
    std::string line;

    // if (!inputFile.is_open()) {
    //     std::cerr << "Error opening file!" << std::endl;
    //     return 1;
    // }

    ControlConfig::set_width(1280);
    ControlConfig::set_height(720);
    ControlConfig::set_fps(30);
    ControlConfig::set_frame_format("mjpeg");
    ControlConfig::set_dwMaxVideoFrameSize(16777216);
    ControlConfig::set_dwMaxPayloadTransferSize(1310720);

    std::thread processing_thread(process_file, std::ref(whole_frame_checker), filename);

#ifdef TUI_SET
    tui();
#elif GUI_SET
    if (start_screen() == -1) {
        processing_thread.join();
        return -1;
    }
    screen();
#endif

    // std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();  // Default current time
    
    
    // while (std::getline(inputFile, line)) {
    //     std::vector<u_char> packet;

    //     // If the line starts with "Converted Monotonic Time:", set the current_time
    //     if (line.find("Converted Monotonic Time:") == 0) {
    //         // Extract the time value from the line
    //         std::string time_string = line.substr(strlen("Converted Monotonic Time: "));
    //         // Convert the time string to a double (representing seconds since the program start)
    //         double monotonic_seconds = std::stod(time_string);
    //         // Convert the seconds to a time_point
    //         current_time = std::chrono::steady_clock::time_point(std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(monotonic_seconds)));
    //         continue;  // Move to the next line, no need to process this line as a packet
    //     }

    //     // Convert line from hex string to packet data
    //     for (size_t i = 0; i < line.length(); i += 2) {
    //         std::string byteString = line.substr(i, 2);
    //         u_char byte = static_cast<u_char>(std::stoi(byteString, nullptr, 16)); 
    //         packet.push_back(byte);  
    //     }

    //     // Use the current_time (either the default or the extracted one)
    //     uint8_t valid_err = whole_frame_checker.payload_valid_ctrl(packet, current_time);
    // }

    processing_thread.join();
    // inputFile.close();

#ifdef GUI_SET
  end_screen();
#endif

    return 0;
}



// int ControlConfig::width = 1280;
// int ControlConfig::height = 720;
// int ControlConfig::fps = 30;
// std::string ControlConfig::frame_format = "mjpeg";
// uint64_t ControlConfig::dwMaxVideoFrameSize = 16777216;
// uint64_t ControlConfig::dwMaxPayloadTransferSize = 1310720;