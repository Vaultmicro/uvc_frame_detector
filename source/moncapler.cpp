#include <chrono>
#include <condition_variable>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>

#include "pcap.h"
#include "validuvc/uvcpheader_checker.hpp"

// #define BULK_USBMON_MAXLENGTHSIZE 16448
// #define ISO_USBMON_MAXLENGTHSIZE 41536 //98496

namespace fs = std::filesystem;

// Global handle for pcap, so it can be closed on exit
// Global log file for the same reason
pcap_t* handle = nullptr;
std::ofstream log_file;

// Global counters for packets and lengths
unsigned int total_packet_count = 0;
unsigned long long total_packet_length = 0;
unsigned long long total_captured_length = 0;
unsigned int filtered_packet_count = 0;
unsigned long long filtered_total_packet_length = 0;
unsigned long long filtered_total_captured_length = 0;
// Global mutex for the queue
std::queue<std::vector<u_char>> packet_queue;
std::mutex queue_mutex;
std::condition_variable queue_cv;
bool stop_processing = false;

// Variables to store user input busnum and devnum
int target_busnum = -1;
int target_devnum = -1;

// USBMON header structure
typedef struct __attribute__((packed, aligned(1))) {
  uint64_t urb_id;               // URB ID (8 bytes, 64 bits)
  uint8_t urb_type;              // URB Type (1 byte, 8 bits)
  uint8_t urb_transfer_type;     // URB Transfer Type (1 byte, 8 bits)
  uint8_t endpoint;              // Endpoint (1 byte, 8 bits)
  uint8_t device_number;         // Device Number (1 byte, 8 bits)
  uint16_t urb_bus_id;           // URB Bus ID (2 bytes, 16 bits)
  uint8_t device_setup_request;  // Device Setup Request (1 byte, 8 bits)
  uint8_t data_present;          // Data Present (1 byte, 8 bits)
  uint64_t urb_sec_hex;          // URB Seconds (8 bytes, 64 bits)
  uint32_t urb_usec_hex;         // URB Microseconds (4 bytes, 32 bits)
  uint32_t urb_status;           // URB Status (4 bytes, 32 bits)
  uint32_t urb_length;           // URB Length (4 bytes, 32 bits)
  uint32_t data_length;          // Data Length (4 bytes, 32 bits)
  union {
    uint64_t setup_data;  // Setup Header (8 bytes, 64 bits)
    struct {
      uint8_t bmRequestType;     // bmRequestType (1 byte, 8 bits)
      uint8_t bRequest;          // bRequest (1 byte, 8 bits)
      uint8_t descriptor_index;  // Descriptor Index (1 byte, 8 bits)
      uint8_t descriptor_type;   // Descriptor Type (1 byte, 8 bits)
      uint16_t language_id;      // Language ID (2 byte, 16 bits)
      uint16_t wLength;          // wLength (2 byte, 16 bits)
    } b_setup_data;
    uint64_t iso_descriptor;  // ISO Descriptor (8 bytes, 64 bits)
    struct {
      uint32_t iso_error_count;  // ISO Packet Descriptor (4 bytes, 32 bits)
      uint32_t iso_descriptor_count;  // ISO Descriptor Count (4 bytes, 32 bits)
    } b_iso_descriptor;
  };
  uint32_t interval;               // Interval (4 bytes, 32 bits)
  uint32_t start_of_frame;         // Start of Frame (4 bytes, 32 bits)
  uint32_t copy_of_transfer_flag;  // Copy of Transfer Flag (4 bytes, 32 bits)
  uint32_t
      iso_descriptor_number;  // Number of ISO Descriptor (4 bytes, 32 bits)
} URB_Data;

// ISO Descriptor structure
typedef struct __attribute__((packed, aligned(1))) {
  uint32_t iso_descriptor_status;
  uint32_t iso_descriptor_offset;
  uint32_t iso_descriptor_length;
  uint32_t iso_descriptor_padding;
} ISO_Descriptor;

void log_packet_xxd_format(std::ofstream* log_file, const u_char* data,
                           int length, int base_address = 0) {
  for (int i = 0; i < length; i += 16) {
    // Print address in hexadecimal
    *log_file << std::setw(8) << std::setfill('0') << std::hex
              << base_address + i << ": ";

    // Print hex values
    for (int j = 0; j < 16; ++j) {
      if (i + j < length) {
        *log_file << std::setw(2) << std::setfill('0') << std::hex
                  << static_cast<int>(data[i + j]) << " ";
      } else {
        *log_file << "   ";  // For aligning ASCII section if data is less than
                             // 16 bytes
      }
    }

    // Print ASCII characters
    *log_file << " ";
    for (int j = 0; j < 16; ++j) {
      if (i + j < length) {
        char ch = static_cast<char>(data[i + j]);
        if (std::isprint(ch)) {
          *log_file << ch;
        } else {
          *log_file << ".";
        }
      }
    }
    *log_file << std::endl;
  }
  *log_file << std::dec;  // Reset to decimal format
}

void coutnlog(const std::string& message, std::ofstream* log_file = nullptr) {
  // Print to console
  std::cout << message << std::endl;

  // Print to log file if it's open
  if (log_file && log_file->is_open()) {
    *log_file << message << std::endl;
  }
}

void print_capture_statistics(const pcap_stat& stats,
                              unsigned int total_packet_count,
                              unsigned long long total_packet_length,
                              unsigned long long total_captured_length,
                              unsigned int filtered_packet_count,
                              unsigned long long filtered_total_packet_length,
                              unsigned long long filtered_total_captured_length,
                              std::ofstream* log_file = nullptr) {
  coutnlog("\n", log_file);
  coutnlog("Capture Statistics:", log_file);
  coutnlog("\n", log_file);
  coutnlog("Total Packets received in usbmon: " + std::to_string(stats.ps_recv),
           log_file);
  coutnlog("Total Packets dropped by system buffer: " +
               std::to_string(stats.ps_drop),
           log_file);
  coutnlog(
      "Total Packets dropped by interface: " + std::to_string(stats.ps_ifdrop),
      log_file);
  coutnlog("\n", log_file);
  coutnlog("Total Packets counted in usbmon by application: " +
               std::to_string(total_packet_count),
           log_file);
  coutnlog("Total Packet Length bytes: " + std::to_string(total_packet_length),
           log_file);
  coutnlog("Total Captured Packet Length bytes: " +
               std::to_string(total_captured_length),
           log_file);
  coutnlog("\n", log_file);
  coutnlog("Filtered Packets (busnum=" + std::to_string(target_busnum) +
               ", devnum=" + std::to_string(target_devnum) +
               "): " + std::to_string(filtered_packet_count),
           log_file);
  coutnlog(
      "Filtered Packet Length bytes (busnum=" + std::to_string(target_busnum) +
          ", devnum=" + std::to_string(target_devnum) +
          "): " + std::to_string(filtered_total_packet_length),
      log_file);
  coutnlog("Filtered Captured Packet Length bytes (busnum=" +
               std::to_string(target_busnum) +
               ", devnum=" + std::to_string(target_devnum) +
               "): " + std::to_string(filtered_total_captured_length),
           log_file);
  coutnlog("\n", log_file);
}

void clean_exit(int signum) {
  struct pcap_stat stats;

  if (handle != nullptr) {
    // Get capture statistics
    if (pcap_stats(handle, &stats) >= 0) {
      // Print capture statistics
      print_capture_statistics(stats, total_packet_count, total_packet_length,
                               total_captured_length, filtered_packet_count,
                               filtered_total_packet_length,
                               filtered_total_captured_length, &log_file);
    } else {
      std::cerr << "pcap_stats failed: " << pcap_geterr(handle) << std::endl;
    }
    pcap_close(handle);
  }

  if (log_file.is_open()) {
    log_file.close();
  }

  std::cout << "Exiting safely..." << std::endl;
  exit(signum);
}

std::string getCurrentTimeFormatted() {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d-%H-%M-%S");
  return oss.str();
}

std::string convertToKST(double unix_timestamp) {
  std::time_t time = static_cast<std::time_t>(unix_timestamp);

  std::tm* gmtm = std::gmtime(&time);

  gmtm->tm_hour += 9;

  if (gmtm->tm_hour >= 24) {
    gmtm->tm_hour -= 24;
    gmtm->tm_mday += 1;
  }

  char buffer[30];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", gmtm);

  std::string result(buffer);
  int milliseconds = static_cast<int>((unix_timestamp - time) * 1000);
  result += "." + std::to_string(milliseconds) + " KST";

  return result;

  // Example usage
  // double timestamp1 = 1724294701.327211;
  // string kst_time1 = convertToKST(timestamp1);
}

void packet_handler(u_char* user_data, const struct pcap_pkthdr* pkthdr,
                    const u_char* packet) {
  static std::vector<u_char> temp_buffer;
  static uint32_t bulk_usbmon_bulk_maxlengthsize;

  std::ofstream* log_file = reinterpret_cast<std::ofstream*>(user_data);

  total_packet_count++;
  total_packet_length += pkthdr->len;
  total_captured_length += pkthdr->caplen;

  // Interpret the packet as a USBMON header
  const URB_Data* urb_data = reinterpret_cast<const URB_Data*>(packet);

  // Extract busnum, devnum, epnum
  int bus_number = static_cast<int>(urb_data->urb_bus_id);
  int device_address = static_cast<int>(urb_data->device_number);
  int endpoint_number = static_cast<int>(
      urb_data->endpoint & 0x7F);  // Extract lower 7 bits for endpoint number
  // string direction = (mon_hdr->epnum & 0x80) ? "Device to Host" : "Host to
  // Device";

  double timestamp = pkthdr->ts.tv_sec + pkthdr->ts.tv_usec / 1e6;
  std::string kst_time = convertToKST(timestamp);

  // Compare busnum and devnum with the target values
  if (bus_number == target_busnum && device_address == target_devnum ||
      target_busnum == -1 && target_devnum == -1 ||
      target_busnum == -1 && device_address == target_devnum ||
      bus_number == target_busnum && target_devnum == -1) {
    filtered_packet_count++;
    filtered_total_packet_length += pkthdr->len;
    filtered_total_captured_length += pkthdr->caplen;

    if (urb_data->urb_status != 0) {
      // std::cerr << "Error detected, skipping this packet" << std::endl;
      return;
    }

    // exclude control transfer
    if (urb_data->urb_transfer_type == 0x02 ||
        urb_data->urb_transfer_type == 0x01) {  // Control Transfer Type (0x02)
      // std::cerr << "Control || Interrupt Transfer detected, skipping this
      // packet" << std::endl;

      // find setcur getcur here
      return;

      // Bulk Transfer Type (0x03)
    } else if (urb_data->urb_transfer_type == 0x03) {
      // std::cout << "Bulk transfer detected" << std::endl;
      if (urb_data->data_length != 16384) {
        std::cout << "Data Length: " << urb_data->data_length << std::endl;
      }

      // std::cout << "Max Length Size: " << bulk_usbmon_bulk_maxlengthsize <<
      // std::endl; std::cout << "size of URB_Data: " << sizeof(URB_Data) <<
      // std::endl;

      // update the max length size
      if (bulk_usbmon_bulk_maxlengthsize <
          urb_data->data_length + sizeof(URB_Data)) {
        if ((urb_data->data_length + sizeof(URB_Data)) % 8 != 0) {
          bulk_usbmon_bulk_maxlengthsize =
              (urb_data->data_length + sizeof(URB_Data) + 8 -
               (urb_data->data_length + sizeof(URB_Data) % 8));
          std::cerr << "Error incorrect max length size: "
                    << bulk_usbmon_bulk_maxlengthsize << std::endl;
        } else {
          bulk_usbmon_bulk_maxlengthsize =
              urb_data->data_length + sizeof(URB_Data);
          std::cout << "update the max length size: "
                    << bulk_usbmon_bulk_maxlengthsize << std::endl;
        }
      }

      if (bulk_usbmon_bulk_maxlengthsize >
          urb_data->data_length + sizeof(URB_Data)) {
        // finish the transfer
        std::cout << "Finish the transfer" << std::endl;

        // TODO make hex format and save them in queue
        temp_buffer.insert(temp_buffer.end(), packet + sizeof(URB_Data),
                           packet + pkthdr->caplen);
        {
          std::lock_guard<std::mutex> lock(queue_mutex);
          packet_queue.push(temp_buffer);
        }
        temp_buffer.clear();
        queue_cv.notify_one();

      } else if (bulk_usbmon_bulk_maxlengthsize ==
                 urb_data->data_length + sizeof(URB_Data)) {
        // continue the transfer
        //  std::cout << "Continue the transfer" << std::endl;
        temp_buffer.insert(temp_buffer.end(), packet + sizeof(URB_Data),
                           packet + pkthdr->caplen);
      } else {
        std::cerr << "Invalid data length for bulk transfer" << std::endl;
        return;
      }

      // Isochronous Transfer (0x00)
    } else if (urb_data->urb_transfer_type == 0x00) {
      // std::cout << "Isochronous transfer detected" << std::endl;

      if (urb_data->iso_descriptor_number > 0) {
        std::vector<ISO_Descriptor> iso_descriptors(
            urb_data->iso_descriptor_number);

        // this create a new pointer to the start of the iso descriptor
        const u_char* iso_descriptor_start = packet + sizeof(URB_Data);
        for (uint8_t i = 0; i < urb_data->iso_descriptor_number; ++i) {
          std::memcpy(&iso_descriptors[i],
                      iso_descriptor_start + i * sizeof(ISO_Descriptor),
                      sizeof(ISO_Descriptor));
        }

        for (uint8_t i = 0; i < urb_data->iso_descriptor_number; ++i) {
          uint32_t start_offset = sizeof(URB_Data) +
                                  (16 * urb_data->iso_descriptor_number) +
                                  iso_descriptors[i].iso_descriptor_offset;
          uint32_t end_offset =
              start_offset + iso_descriptors[i].iso_descriptor_length;

          // checks if the packet is the last packet
          if (i == urb_data->iso_descriptor_number - 1) {
            std::cout << "Last iso descriptor" << std::endl;
            std::cout << end_offset << " " << pkthdr->caplen << std::endl;
          }

          temp_buffer.insert(temp_buffer.end(), packet + start_offset,
                             packet + end_offset);
          {
            std::lock_guard<std::mutex> lock(queue_mutex);
            packet_queue.push(temp_buffer);
          }
          temp_buffer.clear();
          queue_cv.notify_one();
        }
      } else {
        std::cerr << "No iso descriptor detected, skipping this packet"
                  << std::endl;
        return;
      }

    } else {
      std::cerr << "Unknown transfer type detected, skipping this packet"
                << std::endl;
      return;
    }

    // Chrono time
    //  Capture the current time using chrono
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) %
                  1000;
    // Format chrono time
    std::tm local_tm = *std::localtime(&now_time_t);
    char chrono_time_buffer[30];
    std::strftime(chrono_time_buffer, sizeof(chrono_time_buffer),
                  "%Y-%m-%d %H:%M:%S", &local_tm);

    // Log packet information if it matches the target busnum and devnum
    *log_file << "Packet: " << filtered_packet_count
              << " Raw time: " << urb_data->urb_sec_hex << "."
              << urb_data->urb_usec_hex << " Timestamp: " << kst_time
              << " Chrono time: " << chrono_time_buffer << "."
              << std::setfill('0') << std::setw(3) << now_ms.count()
              << " [busnum: " << bus_number << ", devnum: " << device_address
              << ", epnum: " << endpoint_number << "]" << std::endl;


    log_packet_xxd_format(log_file, packet, pkthdr->caplen, 0);
  }
}

void capture_packets() {
  pcap_loop(handle, 0, packet_handler, reinterpret_cast<u_char*>(&log_file));
}

void process_packets() {
  UVCPHeaderChecker header_checker;

  while (true) {
    std::unique_lock<std::mutex> lock(queue_mutex);

    queue_cv.wait(lock,
                  [] { return !packet_queue.empty() || stop_processing; });

    if (stop_processing && packet_queue.empty()) {
      break;
    }

    auto packet = packet_queue.front();
    packet_queue.pop();
    lock.unlock();

    if (!packet.empty()){
      std::cout << "Processing packet of size: " << packet.size() << std::endl;
    }

    uint8_t valid_err = header_checker.payload_valid_ctrl(packet);

    if (valid_err) {
      std::cerr << "Invalid packet detected" << std::endl;
      continue;
    }
    // header_checker.print_packet(packet);
  }
  log_file.close();
}

void test_print_process_packets() {
  std::ofstream log_file("mid_log.log", std::ios::out | std::ios::app);
  if (!log_file) {
    std::cerr << "Failed to open log file" << std::endl;
    return;
  }

  while (true) {
    std::unique_lock<std::mutex> lock(queue_mutex);

    queue_cv.wait(lock,
                  [] { return !packet_queue.empty() || stop_processing; });

    if (stop_processing && packet_queue.empty()) {
      break;
    }

    auto packet = packet_queue.front();
    packet_queue.pop();
    lock.unlock();

    // Test for the packet foramt whether queue is having hex format
    if (!packet.empty() && packet[0] == 0x0c && packet[1] == 0x0c) {
      std::cout << "=================================0x0c." << std::endl;
    }

    if (!packet.empty() && packet[0] == 0x0c && packet[1] == 0x0d) {
      std::cout << "=================================0x0d." << std::endl;
    }
    
    std::cout << "Processing packet of size: " << packet.size() << std::endl;

    for (const auto& byte : packet) {
      log_file << std::hex << std::setw(2) << std::setfill('0')
               << static_cast<int>(byte) << " ";
    }

    log_file << "\n\n";
  }
  log_file.close();
}

int main(int argc, char* argv[]) {
  std::string selected_device;
  int buffer_size = (1280 + 16) * 32 + 64;  // URB LENGTH INCLUDING 64

  // Parse command line arguments
  for (int i = 1; i < argc; i += 2) {
    if (std::strcmp(argv[i], "-in") == 0 && i + 1 < argc) {
      selected_device = argv[i + 1];
    } else if (std::strcmp(argv[i], "-bs") == 0 && i + 1 < argc) {
      buffer_size = std::atoi(argv[i + 1]);
    } else if (std::strcmp(argv[i], "-bn") == 0 && i + 1 < argc) {
      target_busnum = std::atoi(argv[i + 1]);
    } else if (std::strcmp(argv[i], "-dn") == 0 && i + 1 < argc) {
      target_devnum = std::atoi(argv[i + 1]);
    } else {
      std::cerr << "Usage: " << argv[0]
                << " [-in usbmonX] [-bs buffer_size] [-bn busnum] [-dn devnum]"
                << std::endl;
      return 1;
    }
  }

  if (selected_device.empty()) {
    std::cerr << "Error: Device not specified" << std::endl;
    std::cerr << "Usage: " << argv[0]
              << " [-in usbmonX] [-bs buffer_size] [-bn busnum] [-dn devnum]"
              << std::endl;
    return 1;
  }

  if (target_busnum == -1 || target_devnum == -1) {
    std::cout << "busnum or devnum not specified" << std::endl;
    std::cout << "All packets will be captured" << std::endl;
  }

  // Register signal handler for safe exit
  std::signal(SIGINT, clean_exit);
  std::signal(SIGTERM, clean_exit);

  std::cout << "If code is not working try sudo modprobe usbmon" << std::endl;
  std::cout << std::endl;

  struct pcap_stat stats;

  // Get current time formatted string
  std::string current_time_str = getCurrentTimeFormatted();

  // Ensure the log directory exists
  std::string base_path = "./";
  std::string log_dir = base_path + "../log";
  if (!fs::exists(log_dir)) {
    fs::create_directory(log_dir);
  }

  // Find Devices
  char error_buffer[PCAP_ERRBUF_SIZE];
  pcap_if_t *interfaces, *device;

  if (pcap_findalldevs(&interfaces, error_buffer) == -1) {
    std::cerr << "Error finding Device: " << error_buffer << std::endl;
    return 1;
  }

  // Print the list of devices
  int i = 0;
  for (device = interfaces; device != nullptr; device = device->next) {
    std::cout << ++i << ": " << (device->name ? device->name : "No name")
              << std::endl;
    if (device->description)
      std::cout << " (" << device->description << ")" << std::endl;
  }

  // Find the specified device
  for (device = interfaces; device != nullptr; device = device->next) {
    if (selected_device == device->name) {
      break;
    }
  }

  if (device == nullptr) {
    std::cerr << "Error: Device " << selected_device << " not found"
              << std::endl;
    pcap_freealldevs(interfaces);
    return 1;
  }

  handle = pcap_open_live(device->name, buffer_size, 1, 1000, error_buffer);
  if (handle == nullptr) {
    std::cerr << "Error opening device: " << error_buffer << std::endl;
    pcap_freealldevs(interfaces);
    return 1;
  }

  // Log file path
  std::string log_path = log_dir + "/log_pcap_" + current_time_str + ".txt";

  // Open log file
  log_file.open(log_path, std::ios::out);
  if (!log_file) {
    std::cerr << "Failed to open log file" << std::endl;
    return 1;
  }
  std::cout << "Log file created" << std::endl;

  // Free the device list
  pcap_freealldevs(interfaces);

  std::thread capture_thread(capture_packets);

  std::thread process_thread(process_packets);
  // std::thread process_thread(test_print_process_packets);

  std::cout << " Thread started" << std::endl;

  capture_thread.join();
  // // Start packet capture
  // pcap_loop(handle, 0, packet_handler, reinterpret_cast<u_char*>(&log_file));

  {
    std::lock_guard<std::mutex> lock(queue_mutex);
    stop_processing = true;
  }

  queue_cv.notify_one();  // Notify the processing thread
  process_thread.join();

  // This code will not be reached if pcap_loop runs indefinitely
  clean_exit(0);

  return 0;
}
