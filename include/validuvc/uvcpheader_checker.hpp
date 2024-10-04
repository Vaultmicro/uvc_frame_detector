#ifndef UVCPHEADER_CHECKER_HPP
#define UVCPHEADER_CHECKER_HPP

#include <vector>
#include <cctype>
#include <cstdio>
#include <cstdint>
#include <memory>
#include <list>
#include <iomanip>
#include <cstddef>

#include <fstream>
#include <sstream>
#include <bitset>

#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>


#ifdef _WIN32
 #include "winpcap_sdk_include/pcap.h"

#elif __linux__
  #include "pcap.h"
#else
  #include "pcap.h"
#endif


#ifdef _MSC_VER
    #pragma pack(push, 1)
#elif defined(__GNUC__)
    #define PACKED __attribute__((packed, aligned(1)))
#else
    #define PACKED
#endif

typedef struct PACKED {
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

#ifdef _MSC_VER
    #pragma pack(pop) // MSVC에서의 pack 해제
#endif

struct PayloadErrorStats {
    int count_no_error = 0;
    int count_empty_payload = 0;
    int count_max_payload_overflow = 0;
    int count_err_bit_set = 0;
    int count_length_out_of_range = 0;
    int count_length_invalid = 0;
    int count_reserved_bit_set = 0;
    int count_eoh_bit = 0;
    int count_toggle_bit_overlapped = 0;
    int count_fid_mismatch = 0;
    int count_swap = 0;
    int count_missing_eof = 0;
    int count_unknown_error = 0;

    int total() const {
        return count_no_error + count_empty_payload + count_max_payload_overflow +
               count_err_bit_set + count_length_out_of_range + count_length_invalid +
               count_reserved_bit_set + count_eoh_bit + count_toggle_bit_overlapped +
               count_fid_mismatch + count_swap + count_missing_eof + count_unknown_error;
    }

    void print_stats() const {
        int total_count = total();
        std::cout << "Payload Error Statistics:\n";
        std::cout << "No Error: " << count_no_error << " (" << percentage(count_no_error, total_count) << "%)\n";
        std::cout << "Empty Payload: " << count_empty_payload << " (" << percentage(count_empty_payload, total_count) << "%)\n";
        std::cout << "Max Payload Overflow: " << count_max_payload_overflow << " (" << percentage(count_max_payload_overflow, total_count) << "%)\n";
        std::cout << "Error Bit Set: " << count_err_bit_set << " (" << percentage(count_err_bit_set, total_count) << "%)\n";
        std::cout << "Length Out of Range: " << count_length_out_of_range << " (" << percentage(count_length_out_of_range, total_count) << "%)\n";
        std::cout << "Length Invalid: " << count_length_invalid << " (" << percentage(count_length_invalid, total_count) << "%)\n";
        std::cout << "Reserved Bit Set: " << count_reserved_bit_set << " (" << percentage(count_reserved_bit_set, total_count) << "%)\n";
        std::cout << "End of Header Bit: " << count_eoh_bit << " (" << percentage(count_eoh_bit, total_count) << "%)\n";
        std::cout << "Toggle Bit Overlapped: " << count_toggle_bit_overlapped << " (" << percentage(count_toggle_bit_overlapped, total_count) << "%)\n";
        std::cout << "Frame Identifier Mismatch: " << count_fid_mismatch << " (" << percentage(count_fid_mismatch, total_count) << "%)\n";
        std::cout << "Swap: " << count_swap << " (" << percentage(count_swap, total_count) << "%)\n";
        std::cout << "Missing EOF: " << count_missing_eof << " (" << percentage(count_missing_eof, total_count) << "%)\n";
        std::cout << "Unknown Error: " << count_unknown_error << " (" << percentage(count_unknown_error, total_count) << "%)\n";
    }

    double percentage(int count, int total_count) const {
        return total_count == 0 ? 0 : static_cast<double>(count) / total_count * 100.0;
    }
};

struct FrameErrorStats {
    int count_no_error = 0;
    int count_frame_drop = 0;
    int count_frame_error = 0;
    int count_max_frame_overflow = 0;
    int count_invalid_yuyv_raw_size = 0;
    int count_same_different_pts = 0;

    int total() const {
        return count_no_error + count_frame_drop + count_frame_error +
               count_max_frame_overflow + count_invalid_yuyv_raw_size + count_same_different_pts;
    }

    void print_stats() const {
        int total_count = total();
        std::cout << "\nFrame Error Statistics:\n";
        std::cout << "No Error: " << count_no_error << " (" << percentage(count_no_error, total_count) << "%)\n";
        std::cout << "Frame Drop: " << count_frame_drop << " (" << percentage(count_frame_drop, total_count) << "%)\n";
        std::cout << "Frame Error: " << count_frame_error << " (" << percentage(count_frame_error, total_count) << "%)\n";
        std::cout << "Max Frame Overflow: " << count_max_frame_overflow << " (" << percentage(count_max_frame_overflow, total_count) << "%)\n";
        std::cout << "Invalid YUYV Raw Size: " << count_invalid_yuyv_raw_size << " (" << percentage(count_invalid_yuyv_raw_size, total_count) << "%)\n";
        std::cout << "Same Different PTS: " << count_same_different_pts << " (" << percentage(count_same_different_pts, total_count) << "%)\n";
    }

    double percentage(int count, int total_count) const {
        return total_count == 0 ? 0 : static_cast<double>(count) / total_count * 100.0;
    }
};


enum UVCError {
  ERR_NO_ERROR = 0,
  ERR_EMPTY_PAYLOAD = 1,
  ERR_MAX_PAYLAOD_OVERFLOW= 2,
  ERR_ERR_BIT_SET = 3,    // Error bit is set
  ERR_LENGTH_OUT_OF_RANGE = 4, // Invalid header length
  ERR_LENGTH_INVALID = 5, // Invalid length for PTS, Invalid length for SCR
  ERR_RESERVED_BIT_SET = 6, // Reserved bit is set
  ERR_EOH_BIT = 7,
  ERR_TOGGLE_BIT_OVERLAPPED = 8,
  ERR_FID_MISMATCH = 9,    // Frame Identifier mismatch
  ERR_SWAP = 10,
  ERR_MISSING_EOF = 11,

  ERR_UNKNOWN = 99
};

enum FrameError {
  ERR_FRAME_NO_ERROR = 0,
  ERR_FRAME_DROP = 1,
  ERR_FRAME_ERROR = 2,
  ERR_FRAME_MAX_FRAME_OVERFLOW = 3,
  ERR_FRAME_INVALID_YUYV_RAW_SIZE = 4,
  ERR_FRAME_SAME_DIFFERENT_PTS = 5,

};

class ValidFrame{
    public:
        uint32_t frame_number;
        uint16_t packet_number;
        uint32_t frame_pts;
        FrameError frame_error;
        uint8_t eof_reached;

        std::vector<UVC_Payload_Header> payload_headers;  // To store UVC_Payload_Header
        std::vector<size_t> payload_sizes;  // To store the size of each uvc_payload
        
        std::vector<std::chrono::time_point<std::chrono::steady_clock>> received_chrono_times;  // Packet reception times
        std::vector<uint64_t> scr_list;  // System Clock Reference (SCR) list
        std::vector<std::pair<uint32_t, uint32_t>> urb_sec_usec_list;  // URB timestamp list (seconds, microseconds)

        ValidFrame(int frame_num) : frame_number(frame_num), packet_number(0), frame_pts(0), frame_error(ERR_FRAME_NO_ERROR), eof_reached(0) {}

        void add_payload(const UVC_Payload_Header& header, size_t payload_size) {
            payload_headers.push_back(header);  // Add header to the vector
            payload_sizes.push_back(payload_size);  // Add payload size to the vector
            packet_number++;
        }

        void set_frame_error() {
            frame_error = ERR_FRAME_ERROR;
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

    
};

class UVCPHeaderChecker {
    private:

        std::atomic<bool> stop_timer_thread;  
        std::atomic<uint32_t> frame_count;  
        std::thread fps_thread;

        void timer_thread();
        
        UVCError payload_header_valid(const UVC_Payload_Header& payload_header, const UVC_Payload_Header& previous_payload_header, const UVC_Payload_Header& previous_previous_payload_header);
        
        void payload_frame_develope();

        uint32_t current_frame_number;

        PayloadErrorStats payload_stats;
        FrameErrorStats frame_stats;

        void update_payload_error_stat(UVCError error) {
            switch (error) {
                case ERR_NO_ERROR: payload_stats.count_no_error++; break;
                case ERR_EMPTY_PAYLOAD: payload_stats.count_empty_payload++; break;
                case ERR_MAX_PAYLAOD_OVERFLOW: payload_stats.count_max_payload_overflow++; break;
                case ERR_ERR_BIT_SET: payload_stats.count_err_bit_set++; break;
                case ERR_LENGTH_OUT_OF_RANGE: payload_stats.count_length_out_of_range++; break;
                case ERR_LENGTH_INVALID: payload_stats.count_length_invalid++; break;
                case ERR_RESERVED_BIT_SET: payload_stats.count_reserved_bit_set++; break;
                case ERR_EOH_BIT: payload_stats.count_eoh_bit++; break;
                case ERR_TOGGLE_BIT_OVERLAPPED: payload_stats.count_toggle_bit_overlapped++; break;
                case ERR_FID_MISMATCH: payload_stats.count_fid_mismatch++; break;
                case ERR_SWAP: payload_stats.count_swap++; break;
                case ERR_MISSING_EOF: payload_stats.count_missing_eof++; break;
                case ERR_UNKNOWN: payload_stats.count_unknown_error++; break;
                default: break;
            }
        }

        void update_frame_error_stat(FrameError error) {
            switch (error) {
                case ERR_FRAME_NO_ERROR: frame_stats.count_no_error++; break;
                case ERR_FRAME_DROP: frame_stats.count_frame_drop++; break;
                case ERR_FRAME_ERROR: frame_stats.count_frame_error++; break;
                case ERR_FRAME_MAX_FRAME_OVERFLOW: frame_stats.count_max_frame_overflow++; break;
                case ERR_FRAME_INVALID_YUYV_RAW_SIZE: frame_stats.count_invalid_yuyv_raw_size++; break;
                case ERR_FRAME_SAME_DIFFERENT_PTS: frame_stats.count_same_different_pts++; break;
                default: break;
            }
        }

        void save_frames_to_log(std::unique_ptr<ValidFrame>& current_frame);
        void save_payload_header_to_log(
            const UVC_Payload_Header& payload_header,
            std::chrono::time_point<std::chrono::steady_clock> received_time);




    public:
     
        UVCPHeaderChecker()          : stop_timer_thread(false), frame_count(0), current_frame_number(0) {
       fps_thread = std::thread(&UVCPHeaderChecker::timer_thread, this);
        }

        ~UVCPHeaderChecker() {
            std::cout << "UVCPHeaderChecker Destructor" << std::endl;

            stop_timer_thread = true;
            if (fps_thread.joinable()) {
                fps_thread.join();
            }
            print_stats();

        }

        std::list<std::unique_ptr<ValidFrame>> frames;
        std::vector<std::unique_ptr<ValidFrame>> processed_frames;

        uint8_t payload_valid_ctrl(
            const std::vector<u_char>& uvc_payload,
            std::chrono::time_point<std::chrono::steady_clock> received_time);
        
        UVC_Payload_Header parse_uvc_payload_header(
            const std::vector<u_char>& uvc_payload,
            std::chrono::time_point<std::chrono::steady_clock> received_time);

        void frame_valid_ctrl(const std::vector<u_char>& uvc_payload);

        void print_stats() const {
            payload_stats.print_stats();
            frame_stats.print_stats();
            std::cout << std::flush;
        }

};
#endif // UVCPHEADER_CHECKER_HPP