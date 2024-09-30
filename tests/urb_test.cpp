#include <pcap/pcap.h>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <iostream>

// Include necessary header files
#include "moncapler.hpp"

//could be test code err

// Declare global variables (assumed to be extern in packet_handler.cpp)
extern unsigned int total_packet_count;
extern unsigned int filtered_packet_count;
extern unsigned long long total_packet_length;
extern unsigned long long total_captured_length;
extern unsigned long long filtered_total_packet_length;
extern unsigned long long filtered_total_captured_length;
extern unsigned int packet_push_count;


// Function to read packet data from a file
std::vector<u_char> read_packet_data_from_file(const std::string& filename) {
    // Open the file in binary mode
    std::ifstream file(filename, std::ios::binary);
    std::vector<u_char> data;

    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // Read the data
    file.unsetf(std::ios::skipws);
    std::istream_iterator<u_char> begin(file), end;
    data.insert(data.begin(), begin, end);

    return data;
}

// Function to print URB data
void print_urb_data(const URB_Data* urb_data) {
    std::cout << "URB ID: " << std::hex << urb_data->urb_id << std::endl;
    std::cout << "URB Type: " << std::hex << static_cast<int>(urb_data->urb_type) << std::endl;
    std::cout << "URB Transfer Type: " << std::hex << static_cast<int>(urb_data->urb_transfer_type) << std::endl;
    std::cout << "Endpoint: " << std::hex << static_cast<int>(urb_data->endpoint) << std::endl;
    std::cout << "Device Number: " << std::hex << static_cast<int>(urb_data->device_number) << std::endl;
    std::cout << "Bus ID: " << std::hex << urb_data->urb_bus_id << std::endl;
    std::cout << "URB Status: " << std::hex << urb_data->urb_status << std::endl;
    std::cout << "Data Length: " << std::hex << urb_data->data_length << std::endl;
    std::cout << "ISO Descriptor Number: " << std::hex << urb_data->iso_descriptor_number << std::endl;
    std::cout << std::endl;

}

// Function that handles packets and prints URB data
void packet_handler_print(u_char* user_data, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    const URB_Data* urb_data = reinterpret_cast<const URB_Data*>(packet);
    print_urb_data(urb_data);
}

int main() {
    // Read packet data from files
    std::vector<std::string> filenames = {
        "../tests/tph_iso_0.txt",
        "../tests/tph_iso_1.txt",
        "../tests/tph_bulk_0.txt",
        "../tests/tph_bulk_1.txt"
    };

    for (const auto& filename : filenames) {
        try {
            std::vector<u_char> packet_data = read_packet_data_from_file(filename);
            struct pcap_pkthdr pkthdr;
            pkthdr.caplen = packet_data.size();
            pkthdr.len = packet_data.size();
            pkthdr.ts.tv_sec = 0;
            pkthdr.ts.tv_usec = 0;

            std::ofstream log_file("test_log.txt");
            u_char* user_data = reinterpret_cast<u_char*>(&log_file);

            // Call packet_handler_print to print URB data
            packet_handler_print(user_data, &pkthdr, packet_data.data());

            log_file.close();
        } catch (const std::exception& e) {
            std::cerr << "Error reading file: " << filename << " - " << e.what() << std::endl;
        }
    }

    return 0;
}
