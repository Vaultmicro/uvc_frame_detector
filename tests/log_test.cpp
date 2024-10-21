#include <fstream>
#include <iostream>
#include <vector>
#include "validuvc/uvcpheader_checker.hpp"

int main() {
    UVCPHeaderChecker whole_frame_checker;
    std::ifstream inputFile("../tests/change_shark/brio_iso.txt"); 
    std::string line;

    if (!inputFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    while (std::getline(inputFile, line)) {
        std::vector<u_char> packet;

        for (size_t i = 0; i < line.length(); i += 2) {
            std::string byteString = line.substr(i, 2);
            u_char byte = static_cast<u_char>(std::stoi(byteString, nullptr, 16)); 
            packet.push_back(byte);  
        }

        auto current_time = std::chrono::steady_clock::now();
        uint8_t valid_err = whole_frame_checker.payload_valid_ctrl(packet, current_time);

    }

    inputFile.close();
    return 0;
}
