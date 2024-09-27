#include <gtest/gtest.h>
#include <chrono>
#include "validuvc/uvcpheader_checker.hpp"

class frame_test : public ::testing::Test {
protected:
    UVCPHeaderChecker header_checker;

    // Utility function to create a packet with specific data
    std::vector<u_char> create_packet(const std::vector<u_char>& data) {
        return data;
    }
};

// Test for two different frames
TEST_F(frame_test, two_different_frames_test) {
    // First frame packet
    std::vector<u_char> packet_0 = create_packet({
        0x02, 0b10000011, // HLE and BFH (Header Length and Bit Field Header)
        0x00, 0x00, 0x00, 0x00, // PTS (Presentation Time Stamp)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // SCR (Source Clock Reference)
    });

    // Second frame packet
    std::vector<u_char> packet_1 = create_packet({
        0x02, 0b10000010, // HLE and BFH (Header Length and Bit Field Header)
        0x00, 0x00, 0x00, 0x00, // PTS (Presentation Time Stamp)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // SCR (Source Clock Reference)
    });

    auto current_time = std::chrono::steady_clock::now();

    // Process the first frame
    uint8_t valid_err_0 = header_checker.payload_valid_ctrl(packet_0, current_time);
    EXPECT_EQ(valid_err_0, ERR_NO_ERROR);  // Expect no error for the first frame

    // Process the second frame
    uint8_t valid_err_1 = header_checker.payload_valid_ctrl(packet_1, current_time);
    EXPECT_EQ(valid_err_1, ERR_NO_ERROR);  // Expect no error for the second frame

    // Check if frames are properly created and if at least two frames exist
    ASSERT_GE(header_checker.processed_frames.size(), 2) << "There should be at least two frames created.";

    // Use iterator to access elements in std::list
    auto first_frame_number = header_checker.processed_frames[0]->frame_number;
    auto second_frame_number = header_checker.processed_frames[1]->frame_number;

    // Check that two different frames are processed by comparing frame numbers
    ASSERT_NE(first_frame_number, second_frame_number) << "The two frames should have different frame numbers.";
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
