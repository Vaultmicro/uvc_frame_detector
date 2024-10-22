#include <gtest/gtest.h>

#include <chrono>

#include "validuvc/uvcpheader_checker.hpp"

class frame_test : public ::testing::Test {
 protected:
  UVCPHeaderChecker header_checker;
  UVCPHeaderChecker whole_frame_checker;

  // Utility function to create a packet with specific data
  std::vector<u_char> create_packet(const std::vector<u_char>& data) {
    return data;
  }
};

// Test for two different frames
TEST_F(frame_test, two_different_frames_test_insta) {
  // First frame packet
  std::vector<u_char> packet_0 = create_packet({
      0x02, 0b10000011,  // HLE and BFH (Header Length and Bit Field Header)
      0x00, 0x00, 0x00, 0x00,             // PTS (Presentation Time Stamp)
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // SCR (Source Clock Reference)
  });

  // Second frame packet
  std::vector<u_char> packet_1 = create_packet({
      0x02, 0b10000010,  // HLE and BFH (Header Length and Bit Field Header)
      0x00, 0x00, 0x00, 0x00,             // PTS (Presentation Time Stamp)
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // SCR (Source Clock Reference)
  });

  auto current_time = std::chrono::steady_clock::now();

  // Process the first frame
  uint8_t valid_err_0 =
      header_checker.payload_valid_ctrl(packet_0, current_time);
  EXPECT_EQ(valid_err_0, ERR_NO_ERROR);  // Expect no error for the first frame

  // Process the second frame
  uint8_t valid_err_1 =
      header_checker.payload_valid_ctrl(packet_1, current_time);
  EXPECT_EQ(valid_err_1, ERR_NO_ERROR);  // Expect no error for the second frame

  // Check if frames are properly created and if at least two frames exist
  ASSERT_GE(header_checker.processed_frames.size(), 2)
      << "There should be at least two frames created.";

  // Use iterator to access elements in std::list
  auto first_frame_number = header_checker.processed_frames[0]->frame_number;
  auto second_frame_number = header_checker.processed_frames[1]->frame_number;

  // Check that two different frames are processed by comparing frame numbers
  ASSERT_NE(first_frame_number, second_frame_number)
      << "The two frames should have different frame numbers.";
}

// Test for two different frames
TEST_F(frame_test, whole_frame_test_bravia) {
  // First frame packet with JPEG header
  std::vector<u_char> packet_0 = create_packet({
      0x0c, 0b10001101,  // HLE and BFH (Header Length and Bit Field Header)
      0x57, 0x9d, 0x68, 0x00,              // PTS (Presentation Time Stamp)
      0x3e, 0xd2, 0xa6, 0x98, 0xe9, 0x07,  // SCR (Source Clock Reference)

      // JPEG Header
      0xFF, 0xD8,                          // SOI (Start of Image)
      0xFF, 0xE0, 0x00, 0x10,              // APP0 Marker and Length
      0x4A, 0x46, 0x49, 0x46, 0x00,        // "JFIF" Identifier
      0x01, 0x01, 0x00, 0x00, 0x01, 0x00,  // Version and pixel density
      0x01, 0x00, 0x00,                    // Pixel aspect ratio

      0xFF, 0xDB, 0x00, 0x43,        // DQT (Define Quantization Table)
      0x00,                          // Quantization table precision
      0x06, 0x04, 0x04, 0x06, 0x09,  // Quantization table values (part 1)
      0x0E, 0x12, 0x16, 0x04, 0x04,  // Quantization table values (part 2)
      0x05, 0x07, 0x09, 0x15, 0x16,  // Quantization table values (part 3)
      0x14                           // Quantization table values (part 4)
                                     // JPEG data continues as needed
  });

  // Second frame packet
  std::vector<u_char> packet_1 = create_packet({
      0x0c, 0b10001101,  // HLE and BFH (Header Length and Bit Field Header)
      0x57, 0x9d, 0x68, 0x00,              // PTS (Presentation Time Stamp)
      0xd4, 0xd2, 0xa6, 0x98, 0xe9, 0x07,  // SCR (Source Clock Reference)
  });

  // Third frame packet
  std::vector<u_char> packet_2 = create_packet({
      0x0c, 0b10001101,  // HLE and BFH (Header Length and Bit Field Header)
      0x57, 0x9d, 0x68, 0x00,             // PTS (Presentation Time Stamp)
      0x5f, 0xd3, 0xa6, 0x98, 0xe9, 0x07  // SCR (Source Clock Reference)
  });

  // Fourth frame packet
  std::vector<u_char> packet_3 = create_packet({
      0x0c, 0b10001101,  // HLE and BFH (Header Length and Bit Field Header)
      0x57, 0x9d, 0x68, 0x00,             // PTS (Presentation Time Stamp)
      0xdc, 0xd3, 0xa6, 0x98, 0xe9, 0x07  // SCR (Source Clock Reference)
  });

  // Fifth frame packet
  std::vector<u_char> packet_4 = create_packet({
      0x0c, 0b10001101,  // HLE and BFH (Header Length and Bit Field Header)
      0x57, 0x9d, 0x68, 0x00,             // PTS (Presentation Time Stamp)
      0x2e, 0xd5, 0xa6, 0x98, 0xe9, 0x07  // SCR (Source Clock Reference)
  });

  // Sixth frame packet
  std::vector<u_char> packet_5 = create_packet({
      0x0c, 0b10001111,  // HLE and BFH (Header Length and Bit Field Header)
      0x57, 0x9d, 0x68, 0x00,              // PTS (Presentation Time Stamp)
      0xf5, 0xd7, 0xa6, 0x98, 0xea, 0x07,  // SCR (Source Clock Reference)
      0x00, 0x00, 0x00, 0x00, 0xff, 0xd9,  // JPEG data
  });

  // Process the first frame
  auto current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_0 =
      whole_frame_checker.payload_valid_ctrl(packet_0, current_time);
  EXPECT_EQ(valid_err_0, ERR_NO_ERROR);  // Expect no error for the first frame

  // Process the second frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_1 =
      whole_frame_checker.payload_valid_ctrl(packet_1, current_time);
  EXPECT_EQ(valid_err_1, ERR_NO_ERROR);  // Expect no error for the second frame

  // Process the third frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_2 =
      whole_frame_checker.payload_valid_ctrl(packet_2, current_time);
  EXPECT_EQ(valid_err_2, ERR_NO_ERROR);  // Expect no error for the third frame

  // Process the fourth frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_3 =
      whole_frame_checker.payload_valid_ctrl(packet_3, current_time);
  EXPECT_EQ(valid_err_3, ERR_NO_ERROR);  // Expect no error for the fourth frame

  // Process the fifth frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_4 =
      whole_frame_checker.payload_valid_ctrl(packet_4, current_time);
  EXPECT_EQ(valid_err_4, ERR_NO_ERROR);  // Expect no error for the fifth frame

  // Process the sixth frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_5 =
      whole_frame_checker.payload_valid_ctrl(packet_5, current_time);
  EXPECT_EQ(valid_err_5, ERR_NO_ERROR);  // Expect no error for the sixth frame

  // Check if at least one frame is created
  ASSERT_GE(whole_frame_checker.processed_frames.size(), 1)
      << "There should be at least one frame created.";

  std::vector<u_char> packet_6 = create_packet({
      0x0c, 0b10001100,  // HLE and BFH (Header Length and Bit Field Header)
      0x78, 0x9d, 0x68, 0x00,              // PTS (Presentation Time Stamp)
      0x6e, 0x53, 0xa7, 0x98, 0x0a, 0x00,  // SCR (Source Clock Reference)

      // JPEG Header
      0xFF, 0xD8,                          // SOI (Start of Image)
      0xFF, 0xE0, 0x00, 0x10,              // APP0 Marker and Length
      0x4A, 0x46, 0x49, 0x46, 0x00,        // "JFIF" Identifier
      0x01, 0x01, 0x00, 0x00, 0x01, 0x00,  // Version and pixel density
      0x01, 0x00, 0x00,                    // Pixel aspect ratio

      0xFF, 0xDB, 0x00, 0x43,        // DQT (Define Quantization Table)
      0x00,                          // Quantization table precision
      0x06, 0x04, 0x04, 0x06, 0x09,  // Quantization table values (part 1)
      0x0E, 0x12, 0x16, 0x04, 0x04,  // Quantization table values (part 2)
      0x05, 0x07, 0x09, 0x15, 0x16,  // Quantization table values (part 3)
      0x14                           // Quantization table values (part 4)
                                     // JPEG data continues as needed
  });

  current_time = std::chrono::steady_clock::now();

  // Process the first frame
  uint8_t valid_err_6 =
      whole_frame_checker.payload_valid_ctrl(packet_6, current_time);
  EXPECT_EQ(valid_err_6, ERR_NO_ERROR);  // Expect no error for the frame

  // Check if at least one frame is created
  ASSERT_GE(whole_frame_checker.processed_frames.size(), 1)
      << "There should be at least two frame created.";

  // Use iterator to access the first frame's frame number
  auto first_frame_number =
      whole_frame_checker.processed_frames[0]->frame_number;
  auto second_frame_number = whole_frame_checker.frames.front()
                                 ->frame_number; 

  // Check that two different frames are processed by comparing frame numbers
  ASSERT_NE(first_frame_number, second_frame_number)
      << "The two frames should have different frame numbers.";
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
