#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <thread>
#include <chrono>
#include <string>

#include "validuvc/uvcpheader_checker.hpp"

// 전처리 구문 정의
#define ERR_NO_ERROR 0
#define ERR_ERR_BIT_SET 1
#define ERR_LENGTH_OUT_OF_RANGE 2
#define ERR_RESERVED_BIT_SET 3
#define ERR_FID_MISMATCH 4

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
    return hex_string_to_vector(hex_data);  // Convert hex string to vector<u_char>
}

std::vector<u_char> create_packet(int frame_count, const std::vector<u_char>& additional_data) {
    std::vector<u_char> packet;

    // BFH 값을 번갈아 가면서 변경
    if (frame_count % 2 == 0) {
        packet = {
            0x0c, 0b00001111, // HLE and BFH (Header Length and Bit Field Header)
            0x00, 0x00, 0x00, 0x01, // PTS (Presentation Time Stamp)
            0x00, 0x00, 0x00, 0x00, 0x00, 0x01  // SCR (Source Clock Reference)
        };
    } else {
        packet = {
            0x0c, 0b00001110, // HLE and BFH (Header Length and Bit Field Header)
            0x00, 0x00, 0x00, 0x01, // PTS (Presentation Time Stamp)
            0x00, 0x00, 0x00, 0x00, 0x00, 0x01  // SCR (Source Clock Reference)
        };
    }

    //Making Intentional Error

    if (frame_count % 17 == 0){
        packet = {
            0x0c, 0b01001110, // HLE and BFH (Header Length and Bit Field Header)
            0x00, 0x00, 0x00, 0x01, // PTS (Presentation Time Stamp)
            0x00, 0x00, 0x00, 0x00, 0x00, 0x01  // SCR (Source Clock Reference)
        };

    }

    if (frame_count % 100 == 0) {
        packet = {
            0x0c, 0b00001110, // HLE and BFH (Header Length and Bit Field Header)
            0x00, 0x00, 0x00, 0x01, // PTS (Presentation Time Stamp)
            0x00, 0x00, 0x00, 0x00, 0x00, 0x01  // SCR (Source Clock Reference)
        };
    }

    packet.insert(packet.end(), additional_data.begin(), additional_data.end());

    return packet;
}

int main() {
    UVCPHeaderChecker header_checker;

    // 텍스트 파일에서 추가 이미지 데이터를 읽어옴
    std::string filename = "../examples/smpte.txt";
    std::vector<u_char> additional_data = read_additional_image_data(filename);

    if (additional_data.empty()) {
        std::cerr << "No additional image data found in the file." << std::endl;
        return -1;
    }

    int frame_count = 0;

    while (true) {
        // 패킷 생성 함수 호출
        std::vector<u_char> packet = create_packet(frame_count, additional_data);

        uint8_t valid_err = header_checker.payload_valid_ctrl(packet);

        if (valid_err != ERR_NO_ERROR) {
            std::cerr << "===============================================" << std::endl;
            std::cerr << "Error detected: " << static_cast<int>(valid_err) << std::endl;
        }

        // 33ms 대기 (30fps)
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
        frame_count++;
    }

    return 0;
}
