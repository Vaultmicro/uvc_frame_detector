// test_packet_handler.cpp

#include <gtest/gtest.h>
#include <pcap/pcap.h>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <iostream>

// Include necessary header files
#include "moncapler.hpp"

// Declare global variables (assumed to be extern in packet_handler.cpp)
extern unsigned int total_packet_count;
extern unsigned int filtered_packet_count;
extern unsigned long long total_packet_length;
extern unsigned long long total_captured_length;
extern unsigned long long filtered_total_packet_length;
extern unsigned long long filtered_total_captured_length;
extern unsigned int unit_urb_type;
extern unsigned int packet_push_count;


// Function to read packet data from a file
std::vector<u_char> read_packet_data_from_file(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<u_char> data;

    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string hex_str;
    while (file >> hex_str) {
        // 16진수 문자열을 u_char로 변환하여 벡터에 추가
        u_char byte = static_cast<u_char>(std::stoul(hex_str, nullptr, 16)); //stoul makes the hex to u_long type to u_char into 1 byte
        data.push_back(byte);
    }

    return data;
}

 
// Test case for packet_handler function
TEST(PacketHandlerTest, ValidPacket_i0) {
    // Initialize global variables
    total_packet_count = 0;
    filtered_packet_count = 0;
    total_packet_length = 0;
    total_captured_length = 0;
    filtered_total_packet_length = 0;
    filtered_total_captured_length = 0;
    unit_urb_type = 0;
    packet_push_count = 0;


    // 1. Read packet data
    std::string filename = "../tests/tph_iso_0.txt";
    std::vector<u_char> packet_data;

    try {
        packet_data = read_packet_data_from_file(filename);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }

    // 2. Create pcap_pkthdr
    struct pcap_pkthdr pkthdr;
    pkthdr.caplen = packet_data.size();  // Captured packet length
    pkthdr.len = packet_data.size();     // Actual packet length
    // Set timestamp (adjust if necessary)
    pkthdr.ts.tv_sec = 0;
    pkthdr.ts.tv_usec = 0;

    // 3. Set user_data (e.g., log file)
    std::ofstream log_file("test_log.txt");
    u_char* user_data = reinterpret_cast<u_char*>(&log_file);

    // 4. Set necessary global variables (e.g., target_busnum, target_devnum)
    extern int target_busnum;
    extern int target_devnum;
    target_busnum = -1;  // Adjust as needed
    target_devnum = -1;  // Adjust as needed

    // 5. Call packet_handler function
    packet_handler(user_data, &pkthdr, packet_data.data());

    // 6. Verify expected results
    // For example, check if filtered_packet_count is 1
    EXPECT_EQ(filtered_packet_count, 1);
    EXPECT_EQ(packet_push_count, 32) << "packet_queue.push was called " << packet_push_count << " times.";
    EXPECT_EQ(unit_urb_type, 0x43) << "URB_TYPE:  " << unit_urb_type;

    // Close the log file
    log_file.close();
}

// Test case for packet_handler function
TEST(PacketHandlerTest, ValidPacket_i1) {
    // Initialize global variables
    total_packet_count = 0;
    filtered_packet_count = 0;
    total_packet_length = 0;
    total_captured_length = 0;
    filtered_total_packet_length = 0;
    filtered_total_captured_length = 0;
    unit_urb_type = 0;
    packet_push_count = 0;


    // 1. Read packet data
    std::string filename = "../tests/tph_iso_1.txt";
    std::vector<u_char> packet_data;

    try {
        packet_data = read_packet_data_from_file(filename);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }

    // 2. Create pcap_pkthdr
    struct pcap_pkthdr pkthdr;
    pkthdr.caplen = packet_data.size();  // Captured packet length
    pkthdr.len = packet_data.size();     // Actual packet length
    // Set timestamp (adjust if necessary)
    pkthdr.ts.tv_sec = 0;
    pkthdr.ts.tv_usec = 0;

    // 3. Set user_data (e.g., log file)
    std::ofstream log_file("test_log.txt");
    u_char* user_data = reinterpret_cast<u_char*>(&log_file);

    // 4. Set necessary global variables (e.g., target_busnum, target_devnum)
    extern int target_busnum;
    extern int target_devnum;
    target_busnum = -1;  // Adjust as needed
    target_devnum = -1;  // Adjust as needed

    // 5. Call packet_handler function
    packet_handler(user_data, &pkthdr, packet_data.data());

    // 6. Verify expected results
    // For example, check if filtered_packet_count is 1
    EXPECT_EQ(filtered_packet_count, 1);
    EXPECT_EQ(packet_push_count, 32) << "packet_queue.push was called " << packet_push_count << " times.";
    EXPECT_EQ(unit_urb_type, 0x43) << "URB_TYPE:  " << unit_urb_type;

    // Close the log file
    log_file.close();
}


// Test case for packet_handler function
TEST(PacketHandlerTest, ValidPacket_iio) {
    // Initialize global variables
    total_packet_count = 0;
    filtered_packet_count = 0;
    total_packet_length = 0;
    total_captured_length = 0;
    filtered_total_packet_length = 0;
    filtered_total_captured_length = 0;
    unit_urb_type = 0;
    packet_push_count = 0;


    // 1. Read packet data
    std::string filename0 = "../tests/tph_iso_0.txt";
    std::vector<u_char> packet_data;

    try {
        packet_data = read_packet_data_from_file(filename0);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }

   // 2. Create pcap_pkthdr
    struct pcap_pkthdr pkthdr;
    pkthdr.caplen = packet_data.size();  // Captured packet length
    pkthdr.len = packet_data.size();     // Actual packet length
    // Set timestamp (adjust if necessary)
    pkthdr.ts.tv_sec = 0;
    pkthdr.ts.tv_usec = 0;

    // 3. Set user_data (e.g., log file)
    std::ofstream log_file("test_log.txt");
    u_char* user_data = reinterpret_cast<u_char*>(&log_file);

    // 4. Set necessary global variables (e.g., target_busnum, target_devnum)
    extern int target_busnum;
    extern int target_devnum;
    target_busnum = -1;  // Adjust as needed
    target_devnum = -1;  // Adjust as needed

    // 5. Call packet_handler function
    packet_handler(user_data, &pkthdr, packet_data.data());

    // 6. Verify expected results
    // For example, check if filtered_packet_count is 1
    EXPECT_EQ(filtered_packet_count, 1);
    EXPECT_EQ(packet_push_count, 32) << "packet_queue.push was called " << packet_push_count << " times.";
    EXPECT_EQ(unit_urb_type, 0x43) << "URB_TYPE:  " << unit_urb_type;




    std::string filenameo = "../tests/tph_iso_out.txt";

    try {
        packet_data = read_packet_data_from_file(filenameo);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }

    // 2. Create pcap_pkthdr
    pkthdr.caplen = packet_data.size();  // Captured packet length
    pkthdr.len = packet_data.size();     // Actual packet length
    // Set timestamp (adjust if necessary)
    pkthdr.ts.tv_sec = 0;
    pkthdr.ts.tv_usec = 0;

    // 4. Set necessary global variables (e.g., target_busnum, target_devnum)
    extern int target_busnum;
    extern int target_devnum;
    target_busnum = -1;  // Adjust as needed
    target_devnum = -1;  // Adjust as needed

    // 5. Call packet_handler function
    packet_handler(user_data, &pkthdr, packet_data.data());

    // 6. Verify expected results
    // For example, check if filtered_packet_count is 1
    EXPECT_EQ(filtered_packet_count, 2);
    EXPECT_EQ(packet_push_count, 0) << "packet_queue.push was called " << packet_push_count << " times.";
    EXPECT_EQ(unit_urb_type, 0x53) << "URB_TYPE:  " << unit_urb_type;



    
    std::string filename1 = "../tests/tph_iso_1.txt";

    try {
        packet_data = read_packet_data_from_file(filename1);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }

    // 2. Create pcap_pkthdr
    pkthdr.caplen = packet_data.size();  // Captured packet length
    pkthdr.len = packet_data.size();     // Actual packet length
    // Set timestamp (adjust if necessary)
    pkthdr.ts.tv_sec = 0;
    pkthdr.ts.tv_usec = 0;

    // 4. Set necessary global variables (e.g., target_busnum, target_devnum)
    extern int target_busnum;
    extern int target_devnum;
    target_busnum = -1;  // Adjust as needed
    target_devnum = -1;  // Adjust as needed

    // 5. Call packet_handler function
    packet_handler(user_data, &pkthdr, packet_data.data());

    // 6. Verify expected results
    // For example, check if filtered_packet_count is 1
    EXPECT_EQ(filtered_packet_count, 3);
    EXPECT_EQ(packet_push_count, 32) << "packet_queue.push was called " << packet_push_count << " times.";
    EXPECT_EQ(unit_urb_type, 0x43) << "URB_TYPE:  " << unit_urb_type;



    // Close the log file
    log_file.close();
}



// Test case for packet_handler function
TEST(PacketHandlerTest, ValidPacket_b) {
    // Initialize global variables
    total_packet_count = 0;
    filtered_packet_count = 0;
    total_packet_length = 0;
    total_captured_length = 0;
    filtered_total_packet_length = 0;
    filtered_total_captured_length = 0;
    unit_urb_type = 0;
    packet_push_count = 0;


    // 1. Read packet data
    std::string filename0 = "../tests/tph_bulk_0.txt";
    std::vector<u_char> packet_data;

    try {
        packet_data = read_packet_data_from_file(filename0);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }

   // 2. Create pcap_pkthdr
    struct pcap_pkthdr pkthdr;
    pkthdr.caplen = packet_data.size();  // Captured packet length
    pkthdr.len = packet_data.size();     // Actual packet length
    // Set timestamp (adjust if necessary)
    pkthdr.ts.tv_sec = 0;
    pkthdr.ts.tv_usec = 0;

    // 3. Set user_data (e.g., log file)
    std::ofstream log_file("test_log.txt");
    u_char* user_data = reinterpret_cast<u_char*>(&log_file);

    // 4. Set necessary global variables (e.g., target_busnum, target_devnum)
    extern int target_busnum;
    extern int target_devnum;
    target_busnum = -1;  // Adjust as needed
    target_devnum = -1;  // Adjust as needed

    // 5. Call packet_handler function
    packet_handler(user_data, &pkthdr, packet_data.data());

    // 6. Verify expected results
    // For example, check if filtered_packet_count is 1
    EXPECT_EQ(filtered_packet_count, 1);
    EXPECT_EQ(packet_push_count, 0) << "packet_queue.push was called " << packet_push_count << " times.";
    EXPECT_EQ(unit_urb_type, 0x43) << "URB_TYPE:  " << unit_urb_type;




    std::string filename1 = "../tests/tph_bulk_1.txt";

    try {
        packet_data = read_packet_data_from_file(filename1);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }

    // 2. Create pcap_pkthdr
    pkthdr.caplen = packet_data.size();  // Captured packet length
    pkthdr.len = packet_data.size();     // Actual packet length
    // Set timestamp (adjust if necessary)
    pkthdr.ts.tv_sec = 0;
    pkthdr.ts.tv_usec = 0;

    // 4. Set necessary global variables (e.g., target_busnum, target_devnum)
    extern int target_busnum;
    extern int target_devnum;
    target_busnum = -1;  // Adjust as needed
    target_devnum = -1;  // Adjust as needed

    // 5. Call packet_handler function
    packet_handler(user_data, &pkthdr, packet_data.data());

    // 6. Verify expected results
    // For example, check if filtered_packet_count is 1
    EXPECT_EQ(filtered_packet_count, 2);
    EXPECT_EQ(packet_push_count, 1) << "packet_queue.push was called " << packet_push_count << " times.";
    EXPECT_EQ(unit_urb_type, 0x43) << "URB_TYPE:  " << unit_urb_type;


    // Close the log file
    log_file.close();
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
