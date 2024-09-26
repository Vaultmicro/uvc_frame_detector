#ifndef UVCPHEADER_CHECKER_HPP
#define UVCPHEADER_CHECKER_HPP

#include <vector>
#include <cctype>
#include <cstdio>
#include <cstdint>
#include <memory>
#include <list>
#include <iomanip>

#include <fstream>
#include <sstream>
#include <bitset>

#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>

#include "pcap.h"


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
        std::vector<std::chrono::time_point<std::chrono::steady_clock>> received_chrono_times;  // Packet reception times
        std::vector<uint64_t> scr_list;  // System Clock Reference (SCR) list
        std::vector<std::pair<uint32_t, uint32_t>> urb_sec_usec_list;  // URB timestamp list (seconds, microseconds)
        std::chrono::time_point<std::chrono::steady_clock> frame_end_chrono_time;  // Frame end time

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

        void add_received_chrono_time(std::chrono::time_point<std::chrono::steady_clock> time_point) {
            received_chrono_times.push_back(time_point);
        }

        void add_scr(uint64_t scr_value) {
            scr_list.push_back(scr_value);
        }

        void add_urb_sec_usec(uint32_t sec, uint32_t usec) {
            urb_sec_usec_list.emplace_back(sec, usec);
        }

        void set_frame_end_chrono_time(std::chrono::time_point<std::chrono::steady_clock> time_point) {
            frame_end_chrono_time = time_point;
        }

};

class UVCPHeaderChecker {
    private:

        std::atomic<bool> stop_timer_thread;  
        std::atomic<uint32_t> frame_count;  
        std::thread fps_thread;

        void timer_thread() {
            while (!stop_timer_thread) {
                std::this_thread::sleep_for(std::chrono::seconds(1)); 
                std::cout << "FPS: " << frame_count.load() << " frames per second" << std::endl;
                frame_count = 0;
            }
        }
        std::list<std::unique_ptr<ValidFrame>> frames;

        uint8_t payload_header_valid(const UVC_Payload_Header& payload_header, const UVC_Payload_Header& previous_payload_header, const UVC_Payload_Header& previous_previous_payload_header);
        
        void payload_frame_develope();


    public:

        UVCPHeaderChecker() : stop_timer_thread(false), frame_count(0) {
            fps_thread = std::thread(&UVCPHeaderChecker::timer_thread, this);
        }

        ~UVCPHeaderChecker() {
            stop_timer_thread = true;
            if (fps_thread.joinable()) {
                fps_thread.join();
            }
        }


        void print_packet(const std::vector<u_char>& packet);
        uint8_t payload_valid_ctrl(const std::vector<u_char>& uvc_payload, std::chrono::time_point<std::chrono::steady_clock> received_time);
        UVC_Payload_Header parse_uvc_payload_header(const std::vector<u_char>& uvc_payload);

        void frame_valid_ctrl(const std::vector<u_char>& uvc_payload);   
        void save_frames_to_log(std::unique_ptr<ValidFrame>& current_frame);
        void save_payload_header_to_log(const UVC_Payload_Header& payload_header);

};
#endif // UVCPHEADER_CHECKER_HPP