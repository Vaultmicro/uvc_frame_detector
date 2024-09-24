#include "validuvc/uvcpheader_checker.hpp"
#include <iostream>
#include <cstdint>
#include <cstring>

void UVCPHeaderChecker::print_packet(const std::vector<u_char>& packet) {
    for (size_t i = 0; i < packet.size(); i += 16) {
        // printf("%08zx: ", i);

        for (size_t j = 0; j < 16; ++j) {
            if (i + j < packet.size()) {
                printf("%02x ", packet[i + j]);
            } else {
                printf("   "); // padding
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

uint8_t UVCPHeaderChecker::payload_valid_ctrl(const std::vector<u_char>& uvc_payload) {

    //Make graph file
    //Make picture file having mjpeg, yuyv, h264

    if (uvc_payload.empty()) {
        std::cerr << "UVC payload is empty." << std::endl;
        return 1;
    }

    static UVC_Payload_Header previous_previous_payload_header;
    static uint8_t previous_previous_error = 0;
    static UVC_Payload_Header previous_payload_header;
    static uint8_t previous_error = 0;


    UVC_Payload_Header payload_header = parse_uvc_payload_header(uvc_payload);

    uint8_t payload_header_valid_return = payload_header_valid(payload_header, previous_payload_header);

    bool frame_found = false;

    for (auto& frame : frames) {
        if (frame->frame_pts == payload_header.PTS && payload_header.PTS) {
            frame_found = true;
            frame->add_packet(uvc_payload); // if frame found, add packet to the frame

            if (payload_header_valid_return) {
                frame->set_frame_error(); // if error set, add error flag to the frame
            }


            break;
        }
    }

    //create new frame if not found
    if (!frame_found || previous_payload_header.bmBFH.BFH_EOF) {
        frames.push_back(std::make_unique<ValidFrame>(frames.size() + 1));
        auto& new_frame = frames.back();
        new_frame->frame_pts = payload_header.PTS; //frame pts == payload pts
        new_frame->add_packet(uvc_payload);
        
        if (payload_header_valid_return) {
            new_frame->set_frame_error();
        }
    }

    //Check if the previous frame has an error and switch
    if (!previous_error){
        previous_payload_header = payload_header;
    } 


    if (payload_header.bmBFH.BFH_EOF) {
        //TODO
        //finish the frame
        //release memory 
    }

    if (payload_header_valid_return) {
        //TODO save in the error frame heap
        return 1;
    }

    uint8_t previous_fid = payload_header.bmBFH.BFH_FID;

    save_frames_to_log("frames_log.txt");

    std::cout << "Payload is valid." << std::endl;
    return 0;
}

UVC_Payload_Header UVCPHeaderChecker::parse_uvc_payload_header(const std::vector<u_char>& uvc_payload) {

    UVC_Payload_Header payload_header;
    if (uvc_payload.size() < 2) {
        std::cerr << "Error: UVC payload size is too small." << std::endl;
        return payload_header; // check if payload is too small for payload header
    }

    // copy hle, bfh
    payload_header.HLE = uvc_payload[0];
    payload_header.BFH = uvc_payload[1];

    size_t current_offset = 2;

    if (payload_header.bmBFH.BFH_PTS && current_offset + sizeof(uint32_t) <= uvc_payload.size()) {
        // pts if present
        std::memcpy(&payload_header.PTS, &uvc_payload[current_offset], sizeof(uint32_t));
        current_offset += sizeof(uint32_t);
    } else {
        payload_header.PTS = 0;
    }

    if (payload_header.bmBFH.BFH_SCR && current_offset + sizeof(uint64_t) <= uvc_payload.size()) {
        // scr if present
        std::memcpy(&payload_header.SCR, &uvc_payload[current_offset], sizeof(uint64_t));
    } else {
        payload_header.SCR = 0;
    }

    save_payload_header_to_log(payload_header);

    return payload_header;
}

uint8_t UVCPHeaderChecker::payload_header_valid(const UVC_Payload_Header& payload_header, const UVC_Payload_Header& previous_payload_header) {

    //Checks if the header length is valid
    if (payload_header.HLE < 0x02 || payload_header.HLE > 0x0C) {
        std::cerr << "Invalid UVC payload header: Unexpected start byte 0x" 
                  << std::hex << static_cast<int>(payload_header.HLE) << "." << std::endl;
        return 1;
    }

    //Checks if the Frame Identifier bit is set
    if (payload_header.bmBFH.BFH_FID == previous_payload_header.bmBFH.BFH_FID && (payload_header.PTS != previous_payload_header.PTS) || payload_header.PTS == 0){
        std::cerr << "Invalid UVC payload header: Frame Identifier bit is not the same as the previous frame." << std::endl;
        return 1;
    }

    //Checks if the End of Frame bit is set
    //TODO
    //Check with the total length of the frame and the calculated length of the frame

    //Checks if the Presentation Time Stamp bit is set
    //Checks if the Source Clock Reference bit is set
    if (payload_header.bmBFH.BFH_PTS && payload_header.bmBFH.BFH_SCR && payload_header.HLE != 0x0C) {
        std::cerr << "Invalid UVC payload header: Both Presentation Time Stamp and Source Clock Reference bits are set." << std::endl;
        return 1;
    } else if (payload_header.bmBFH.BFH_PTS && !payload_header.bmBFH.BFH_SCR && payload_header.HLE != 0x06) {
        std::cerr << "Invalid UVC payload header: Presentation Time Stamp bit is set but header length is less than 6." << std::endl;
        return 1;
    } else if (!payload_header.bmBFH.BFH_PTS && payload_header.bmBFH.BFH_SCR && payload_header.HLE != 0x08) {
        std::cerr << "Invalid UVC payload header: Source Clock Reference bit is set but header length is less than 12." << std::endl;
        return 1;
    } else if (!payload_header.bmBFH.BFH_PTS && !payload_header.bmBFH.BFH_SCR && payload_header.HLE != 0x02) {
        std::cerr << "Invalid UVC payload header: Neither Presentation Time Stamp nor Source Clock Reference bits are set but header length is not 2." << std::endl;
        return 1;
    }

    //Checks if the Reserved bit is set
    if (payload_header.bmBFH.BFH_RES) {
        std::cerr << "Invalid UVC payload header: Reserved bit is set." << std::endl;
        return 1;
    }

    //Checks if the Still Image bit is set


    //Checks if the Error bit is set
    if (payload_header.bmBFH.BFH_ERR) {
        std::cerr << "Invalid UVC payload header: Error bit is set." << std::endl;
        return 1;
    }

    // //Checks if the End of Header bit is set
    // if (!payload_header.bmBFH.BFH_EOH) {
    //     std::cerr << "Invalid UVC payload header: End of Header (EOH) bit is not set." << std::endl;
    //     return 1;
    // }

    std::cout << "UVC payload header is valid." << std::endl;
    return 0;

}


void UVCPHeaderChecker::frame_valid_ctrl(const std::vector<u_char>& uvc_payload){
    //Save the format of the frame
    //Save them on the multiple stack with the time, so err frame be detected and pop from the stack
    //The algorithm to detect the error frame is to compare the frame with the previous frame
}


void UVCPHeaderChecker::payload_frame_develope(){
    //Call the picture file
}


void UVCPHeaderChecker::save_frames_to_log(const std::string& filename) {
    std::ofstream log_file(filename, std::ios::app);

    if (!log_file.is_open()) {
        std::cerr << "Error opening log file." << std::endl;
        return;
    }

    for (const auto& frame : frames) {
        std::stringstream frame_info;
        frame_info << "Frame Number: " << frame->frame_number << "\n"
                   << "Packet Number: " << frame->packet_number << "\n"
                   << "Frame PTS: " << frame->frame_pts << "\n"
                   << "Frame Error: " << static_cast<int>(frame->frame_error) << "\n"
                   << "EOF Reached: " << static_cast<int>(frame->eof_reached) << "\n"
                   << "Packets:\n";

        for (const auto& packet : frame->packets) {
            frame_info << "  Packet Size: " << packet.size() << " bytes\n";
        }

        log_file << frame_info.str() << "\n---\n";
    }

    log_file.close();
}

void UVCPHeaderChecker::save_payload_header_to_log(const UVC_Payload_Header& payload_header) {
    std::ofstream log_file("payload_headers_log.txt", std::ios::app);

    if (!log_file.is_open()) {
        std::cerr << "Error opening payload header log file." << std::endl;
        return;
    }

    log_file << std::hex; // Set the output stream to hexadecimal mode
    log_file << "HLE: " << std::setw(2) << std::setfill('0') << static_cast<int>(payload_header.HLE) << "\n";

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

    log_file << "PTS: " << std::setw(8) << std::setfill('0') << payload_header.PTS << "\n";
    log_file << "SCR:\n"
             << "  STC: " << static_cast<int>(payload_header.bmSCR.SCR_STC) << "\n"
             << "  TOK: " << std::bitset<11>(payload_header.bmSCR.SCR_TOK) << "\n"
             << "  RES: " << std::bitset<5>(payload_header.bmSCR.SCR_RES) << "\n"
             << "\n\n";  // Separate each entry with a double newline


    log_file.close();
}
