#ifndef UVCPHEADER_CHECKER_HPP
#define UVCPHEADER_CHECKER_HPP

#include <vector>
#include <cctype>
#include <cstdio>
#include <cstdint>
#include <memory>
#include <list>
#include "pcap.h"
#include <iomanip>

#include <fstream>
#include <sstream>
#include <bitset>

typedef struct __attribute__((packed, aligned(1))) {
    uint8_t HLE;                       // Header Length
    union {
        uint8_t BFH;                   // Bit Field Header
        struct {
            uint8_t BFH_FID : 1;       // Frame Identifier
            uint8_t BFH_EOF : 1;       // End of Frame
            uint8_t BFH_PTS : 1;       // Presentation Time Stamp
            uint8_t BFH_SCR : 1;       // Source Clock Reference
            uint8_t BFH_RES : 1;       // Reserved
            uint8_t BFH_STI : 1;       // Still Image
            uint8_t BFH_ERR : 1;       // Error
            uint8_t BFH_EOH : 1;       // End of Header
        } bmBFH;                       
    };
    uint32_t PTS;                      // Presentation Time Stamp 32bits
    union {
        uint64_t SCR;                  // Clock Reference 48bits
        struct {
            uint32_t SCR_STC : 32;     // System Time Clock, 32 bits
            uint16_t SCR_TOK : 11;     // Token Counter, 11 bits
            uint16_t SCR_RES : 5;      // Reserved 0, 5 bits
        } bmSCR;                       // Bitfield for SCR
    };
} UVC_Payload_Header;

enum UVCError {
  ERR_NO_ERROR = 0,
  ERR_ERR_BIT_SET = 1,    // Error bit is set
  ERR_LENGTH_OUT_OF_RANGE = 2, // Invalid header length

  ERR_LENGTH_INVALID = 4, // Invalid length for PTS, Invalid length for SCR
  ERR_SCR_LENGTH_INVALID = 5, // 
  ERR_NO_PTS_SCR_LENGTH_INVALID = 6, // Neither PTS nor SCR but invalid length
  ERR_RESERVED_BIT_SET = 7, // Reserved bit is set
  ERR_FID_MISMATCH = 8,    // Frame Identifier mismatch
};

class ValidFrame{
    public:
        uint64_t frame_number;
        uint16_t packet_number;
        uint32_t frame_pts;
        uint8_t frame_error;
        uint8_t eof_reached;

        std::vector<std::vector<u_char>> packets;

        ValidFrame(int frame_num) : frame_number(frame_num), packet_number(0), frame_pts(0), frame_error(0), eof_reached(0) {}

        void add_packet(const std::vector<u_char>& packet) {
            packets.push_back(packet);
            packet_number++;
        }

        void set_frame_error() {
            frame_error = 1;
        }

        void set_eof_reached() {
            eof_reached = 1;
        }
};

class UVCPHeaderChecker {
    private:
        std::list<std::unique_ptr<ValidFrame>> frames;

        uint8_t payload_header_valid(const UVC_Payload_Header& payload_header, const UVC_Payload_Header& previous_payload_header, const UVC_Payload_Header& previous_previous_payload_header);
        
        void payload_frame_develope();


    public:
        void print_packet(const std::vector<u_char>& packet);
        uint8_t payload_valid_ctrl(const std::vector<u_char>& uvc_payload);
        UVC_Payload_Header parse_uvc_payload_header(const std::vector<u_char>& uvc_payload);

        void frame_valid_ctrl(const std::vector<u_char>& uvc_payload);   
        void save_frames_to_log(std::unique_ptr<ValidFrame>& current_frame);
        void save_payload_header_to_log(const UVC_Payload_Header& payload_header);


};
#endif // UVCPHEADER_CHECKER_HPP