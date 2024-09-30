#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "validuvc/control_config.hpp"
#include "validuvc/uvcpheader_checker.hpp"

extern int verbose_level;

// Utility function to convert a string of hex values to a vector of u_char
std::vector<u_char> hex_string_to_vector(const std::string& hex_string) {
  std::vector<u_char> result;
  std::stringstream ss(hex_string);
  std::string hex_byte;

  // Parse each hex byte and convert it to u_char
  while (ss >> hex_byte) {
    result.push_back(static_cast<u_char>(std::stoul(hex_byte, nullptr, 16)));
  }

  return result;
}

// Function to read only the image data from a text file
std::vector<u_char> read_additional_image_data(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Unable to open file: " << filename << std::endl;
    return {};
  }

  std::stringstream buffer;
  buffer << file.rdbuf();  // Read entire file content into a stringstream
  file.close();

  std::string hex_data = buffer.str();
  return hex_string_to_vector(
      hex_data);  // Convert hex string to vector<u_char>
}

std::vector<u_char> create_packet(int frame_count,
                                  const std::vector<u_char>& additional_data) {
  std::vector<u_char> packet;

  // BFH switch
  if (frame_count % 2 == 0) {
    packet = {
        0x02, 0b00000011,  // HLE and BFH (Header Length and Bit Field Header)
        0x00, 0x00,       0x00, 0x00,  // PTS (Presentation Time Stamp)
        0x00, 0x00,       0x00, 0x00,
        0x00, 0x00  // SCR (Source Clock Reference)
    };
  } else {
    packet = {
        0x02, 0b00000010,  // HLE and BFH (Header Length and Bit Field Header)
        0x00, 0x00,       0x00, 0x00,  // PTS (Presentation Time Stamp)
        0x00, 0x00,       0x00, 0x00,
        0x00, 0x00  // SCR (Source Clock Reference)
    };
  }

  // Making Intentional Error

  // if (frame_count % 10 == 1){//17
  //     packet = {
  //         0x0c, 0b01001110, // HLE and BFH (Header Length and Bit Field
  //         Header) 0x00, 0x00, 0x00, 0x01, // PTS (Presentation Time Stamp)
  //         0x00, 0x00, 0x00, 0x00, 0x00, 0x01  // SCR (Source Clock Reference)
  //     };

  // }

  // if (frame_count % 100 == 0) {
  //     packet = {
  //         0x0c, 0b00001110, // HLE and BFH (Header Length and Bit Field
  //         Header) 0x00, 0x00, 0x00, 0x01, // PTS (Presentation Time Stamp)
  //         0x00, 0x00, 0x00, 0x00, 0x00, 0x01  // SCR (Source Clock Reference)
  //     };
  // }

  packet.insert(packet.end(), additional_data.begin(), additional_data.end());

  return packet;
}

int main(int argc, char* argv[]) {
  verbose_level = 2;

  UVCPHeaderChecker header_checker;

  ControlConfig::set_width(1080);
  ControlConfig::set_height(720);
  ControlConfig::set_fps(60);
  ControlConfig::set_frame_format("MJPEG");

  static uint8_t designed_fps = 30;
  std::chrono::milliseconds frame_interval(1000 / designed_fps);  // 33ms
  std::chrono::steady_clock::time_point next_frame_time =
      std::chrono::steady_clock::now();

  // smpte from txt
  std::string filename = "../examples/smpte.txt";
  std::vector<u_char> additional_data = read_additional_image_data(filename);

  if (additional_data.empty()) {
    std::cerr << "No additional image data found in the file." << std::endl;
    return -1;
  }

  int frame_count = 0;

  while (true) {
    auto current_time = std::chrono::steady_clock::now();
    if (current_time < next_frame_time) {
      std::this_thread::sleep_for(next_frame_time - current_time);
    }

    std::vector<u_char> packet = create_packet(frame_count, additional_data);

    auto received_time = std::chrono::steady_clock::now();

    uint8_t valid_err =
        header_checker.payload_valid_ctrl(packet, received_time);

    if (valid_err != ERR_NO_ERROR) {
      std::cerr << "==============================================="
                << std::endl;
      std::cerr << "Error detected: " << static_cast<int>(valid_err)
                << std::endl;
    }

    next_frame_time += frame_interval;
    frame_count++;
  }

  return 0;
}
