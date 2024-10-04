#ifndef MONCAPLER_HPP
#define MONCAPLER_HPP

#include <chrono>
#include <condition_variable>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
  #include <filesystem>


#ifdef _WIN32
 #include "winpcap_sdk_include/pcap.h"

#elif __linux__
  #include "pcap.h"
#else
  #include "pcap.h"
#endif

#include "utils/logger.hpp"
#include "utils/verbose.hpp"
#include "validuvc/control_config.hpp"
#include "validuvc/uvcpheader_checker.hpp"

namespace fs = std::filesystem;

// External variables used across the project
extern pcap_t* handle;
extern std::ofstream log_file;
extern unsigned int total_packet_count;
extern unsigned long long total_packet_length;
extern unsigned long long total_captured_length;
extern unsigned int filtered_packet_count;
extern unsigned long long filtered_total_packet_length;
extern unsigned long long filtered_total_captured_length;

extern std::queue<std::chrono::time_point<std::chrono::steady_clock>>
    time_records;
extern std::mutex time_mutex;

extern std::queue<std::vector<u_char>> packet_queue;
extern std::mutex queue_mutex;
extern std::condition_variable queue_cv;
extern bool stop_processing;

extern int verbose_level;
extern int log_verbose_level;

extern int target_busnum;
extern int target_devnum;

#ifdef _MSC_VER
    #pragma pack(push, 1)
    #define PACKED 
#elif defined(__GNUC__)
    #define PACKED __attribute__((packed, aligned(1)))
#else
    #define PACKED
#endif

typedef struct PACKED {
  uint64_t urb_id;
  uint8_t urb_type;
  uint8_t urb_transfer_type;
  uint8_t endpoint;
  uint8_t device_number;
  uint16_t urb_bus_id;
  uint8_t device_setup_request;
  uint8_t data_present;
  uint64_t urb_sec_hex;
  uint32_t urb_usec_hex;
  uint32_t urb_status;
  uint32_t urb_length;
  uint32_t data_length;
  union {
    uint64_t setup_data;
    struct {
      uint8_t bmRequestType;
      uint8_t bRequest;
      uint8_t descriptor_index;
      uint8_t descriptor_type;
      uint16_t language_id;
      uint16_t wLength;
    } b_setup_data;
    uint64_t iso_descriptor;
    struct {
      uint32_t iso_error_count;
      uint32_t iso_descriptor_count;
    } b_iso_descriptor;
  };
  uint32_t interval;
  uint32_t start_of_frame;
  uint32_t copy_of_transfer_flag;
  uint32_t iso_descriptor_number;
} URB_Data;

typedef struct PACKED {
  uint32_t iso_descriptor_status;
  uint32_t iso_descriptor_offset;
  uint32_t iso_descriptor_length;
  uint32_t iso_descriptor_padding;
} ISO_Descriptor;


#ifdef _MSC_VER
    #pragma pack(pop) // MSVC에서의 pack 해제
#endif


// Function declarations
void log_packet_xxd_format(std::ofstream* log_file, const u_char* data,
                           int length, int base_address = 0);
void coutnlog(const std::string& message, std::ofstream* log_file = nullptr);
void print_capture_statistics(const pcap_stat& stats,
                              unsigned int total_packet_count,
                              unsigned long long total_packet_length,
                              unsigned long long total_captured_length,
                              unsigned int filtered_packet_count,
                              unsigned long long filtered_total_packet_length,
                              unsigned long long filtered_total_captured_length,
                              std::ofstream* log_file = nullptr);
void clean_exit(int signum);
std::string getCurrentTimeFormatted();
std::string convertToKST(double unix_timestamp);
void packet_handler(u_char* user_data, const struct pcap_pkthdr* pkthdr,
                    const u_char* packet);
void capture_packets();
void process_packets();
void test_print_process_packets();

#endif  // MONCAPLER_HPP
