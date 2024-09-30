#include <gtest/gtest.h>

#include <chrono>

#include "validuvc/uvcpheader_checker.hpp"

class frame_test : public ::testing::Test {
 protected:
  UVCPHeaderChecker whole_frame_checker;

  // Utility function to create a packet with specific data
  std::vector<u_char> create_packet(const std::vector<u_char>& data) {
    return data;
  }
};

// Test for whole frame
TEST_F(frame_test, whole_frame_test_brio) {
  // Packet 0
  std::vector<u_char> packet_0 = create_packet(
      {0x0c, 0x0c, 0x37, 0x1e, 0x96, 0xa8, 0x19, 0x71, 0x9d, 0xa8, 0x00, 0x00,
       0x87, 0x80, 0x87, 0x80, 0x87, 0x80, 0x87, 0x80, 0x86, 0x7f, 0x86, 0x80,
       0x85, 0x7f, 0x85, 0x80, 0x84, 0x7f, 0x83, 0x7f, 0x83, 0x7f, 0x83, 0x7f,
       0x83, 0x80, 0x83, 0x7f, 0x82, 0x80, 0x81, 0x7f, 0x81, 0x80, 0x80, 0x7f,
       0x80, 0x80, 0x7f, 0x80, 0x7e, 0x80, 0x7e, 0x80, 0x7d, 0x80, 0x7d, 0x7f,
       0x7c, 0x7f, 0x7b, 0x7f, 0x7b, 0x7f, 0x7b, 0x7f, 0x7b, 0x7f, 0x7a, 0x7f,
       0x7a, 0x7f, 0x79, 0x80, 0x79, 0x7f});

  // Packet 1
  std::vector<u_char> packet_1 = create_packet(
      {0x0c, 0x0c, 0x37, 0x1e, 0x96, 0xa8, 0x19, 0x71, 0x9d, 0xa8, 0x00, 0x00,
       0x78, 0x7f, 0x78, 0x7f, 0x79, 0x7f, 0x79, 0x7f, 0x79, 0x7f, 0x7a, 0x7f,
       0x7a, 0x7f, 0x7a, 0x7f, 0x7b, 0x7f, 0x7b, 0x7f, 0x7b, 0x7f, 0x7b, 0x7f,
       0x7c, 0x7f, 0x7c, 0x7f, 0x7d, 0x7f, 0x7e, 0x7f, 0x7e, 0x7f, 0x7f, 0x7f,
       0x7f, 0x7f, 0x80, 0x7f, 0x81, 0x7f, 0x82, 0x7f, 0x83, 0x7f, 0x83, 0x7f,
       0x84, 0x7f, 0x85, 0x7f, 0x86, 0x80, 0x86, 0x7f});

  // Packet 2
  std::vector<u_char> packet_2 = create_packet(
      {0x0c, 0x0c, 0x37, 0x1e, 0x96, 0xa8, 0x19, 0x71, 0x9d, 0xa8, 0x00, 0x00,
       0x78, 0x7e, 0x78, 0x7f, 0x77, 0x7e, 0x78, 0x7f, 0x78, 0x7f, 0x78, 0x7f,
       0x78, 0x7f, 0x78, 0x7f, 0x78, 0x7f, 0x78, 0x7f, 0x78, 0x7f, 0x78, 0x7f,
       0x77, 0x7f, 0x78, 0x7f, 0x77, 0x7f, 0x78, 0x7f, 0x78, 0x7f, 0x78, 0x7f,
       0x78, 0x7f, 0x78, 0x7f, 0x78, 0x7f, 0x78, 0x7f});

  // Packet 3
  std::vector<u_char> packet_3 = create_packet(
      {0x0c, 0x0c, 0x37, 0x1e, 0x96, 0xa8, 0x19, 0x71, 0x9d, 0xa8, 0x00, 0x00});

  // Packet 4
  std::vector<u_char> packet_4 = create_packet(
      {0x0c, 0x1e, 0x37, 0x1e, 0x96, 0xa8, 0x85, 0x19, 0xac, 0xa8, 0x00, 0x00});

  // Packet 5
  std::vector<u_char> packet_5 = create_packet(
      {0x0c, 0x0d, 0x37, 0x1e, 0x96, 0xa8, 0x43, 0x8c, 0xac, 0xa8, 0x00, 0x00});

  // Packet 6
  std::vector<u_char> packet_6 = create_packet(
      {0x0c, 0x0d, 0x37, 0x1e, 0x96, 0xa8, 0x43, 0x8c, 0xac, 0xa8, 0x00, 0x00});

  // Packet 7
  std::vector<u_char> packet_7 = create_packet(
      {0x0c, 0x0d, 0x37, 0x1e, 0x96, 0xa8, 0x43, 0x8c, 0xac, 0xa8, 0x00, 0x00});

  // Packet 8
  std::vector<u_char> packet_8 = create_packet(
      {0x0c, 0x0d, 0x37, 0x1e, 0x96, 0xa8, 0x43, 0x8c, 0xac, 0xa8, 0x00, 0x00});

  // Packet 9
  std::vector<u_char> packet_9 = create_packet(
      {0x0c, 0x0d, 0x37, 0x1e, 0x96, 0xa8, 0x43, 0x8c, 0xac, 0xa8, 0x00, 0x00});

  // Packet 10
  std::vector<u_char> packet_10 = create_packet(
      {0x0c, 0x0d, 0x37, 0x1e, 0x96, 0xa8, 0x43, 0x8c, 0xac, 0xa8, 0x00, 0x00});

  // Packet 11
  std::vector<u_char> packet_11 = create_packet(
      {0x0c, 0x0d, 0x37, 0x1e, 0x96, 0xa8, 0x43, 0x8c, 0xac, 0xa8, 0x00, 0x00});

  // Packet 12
  std::vector<u_char> packet_12 = create_packet(
      {0x0c, 0x0d, 0x37, 0x1e, 0x96, 0xa8, 0x43, 0x8c, 0xac, 0xa8, 0x00, 0x00});

  // Packet 13
  std::vector<u_char> packet_13 = create_packet(
      {0x0c, 0x0d, 0x37, 0x1e, 0x96, 0xa8, 0x43, 0x8c, 0xac, 0xa8, 0x00, 0x00});

  // Packet 14
  std::vector<u_char> packet_14 = create_packet(
      {0x0c, 0x0d, 0x37, 0x1e, 0x96, 0xa8, 0x43, 0x8c, 0xac, 0xa8, 0x00, 0x00});

  // Packet 15
  std::vector<u_char> packet_15 = create_packet(
      {0x0c, 0x0d, 0xdf, 0x4e, 0xa5, 0xa8, 0x43, 0x8c, 0xac, 0xa8, 0x00, 0x00,
       0xe9, 0x7f, 0xe9, 0x7f, 0xe9, 0x80, 0xe9, 0x80, 0xe9, 0x80, 0xe9, 0x80});

  // Packet 16
  std::vector<u_char> packet_16 = create_packet(
      {0x0c, 0x0d, 0xdf, 0x4e, 0xa5, 0xa8, 0x43, 0x8c, 0xac, 0xa8, 0x00, 0x00,
       0x31, 0x83, 0x30, 0x80, 0x2f, 0x83, 0x2e, 0x80, 0x2d, 0x83, 0x2c, 0x80});

  // Packet 17
  std::vector<u_char> packet_17 = create_packet(
      {0x0c, 0x0d, 0xdf, 0x4e, 0xa5, 0xa8, 0x43, 0x8c, 0xac, 0xa8, 0x00, 0x00,
       0x73, 0x82, 0x72, 0x80, 0x72, 0x82, 0x72, 0x80, 0x72, 0x82, 0x71, 0x80});

  // Packet 18
  std::vector<u_char> packet_18 = create_packet(
      {0x0c, 0x0d, 0xdf, 0x4e, 0xa5, 0xa8, 0x43, 0x8c, 0xac, 0xa8, 0x00, 0x00,
       0xb4, 0x75, 0xb4, 0x8c, 0xb5, 0x74, 0xb5, 0x8e, 0xb5, 0x71, 0xb4, 0x90});

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

  // Process the seventh frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_6 =
      whole_frame_checker.payload_valid_ctrl(packet_6, current_time);
  EXPECT_EQ(valid_err_6,
            ERR_NO_ERROR);  // Expect no error for the seventh frame

  // Process the eighth frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_7 =
      whole_frame_checker.payload_valid_ctrl(packet_7, current_time);
  EXPECT_EQ(valid_err_7, ERR_NO_ERROR);  // Expect no error for the eighth frame

  // Process the ninth frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_8 =
      whole_frame_checker.payload_valid_ctrl(packet_8, current_time);
  EXPECT_EQ(valid_err_8, ERR_NO_ERROR);  // Expect no error for the ninth frame

  // Process the tenth frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_9 =
      whole_frame_checker.payload_valid_ctrl(packet_9, current_time);
  EXPECT_EQ(valid_err_9, ERR_NO_ERROR);  // Expect no error for the tenth frame

  // Process the eleventh frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_10 =
      whole_frame_checker.payload_valid_ctrl(packet_10, current_time);
  EXPECT_EQ(valid_err_10,
            ERR_NO_ERROR);  // Expect no error for the eleventh frame

  // Process the twelfth frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_11 =
      whole_frame_checker.payload_valid_ctrl(packet_11, current_time);
  EXPECT_EQ(valid_err_11,
            ERR_NO_ERROR);  // Expect no error for the twelfth frame

  // Process the thirteenth frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_12 =
      whole_frame_checker.payload_valid_ctrl(packet_12, current_time);
  EXPECT_EQ(valid_err_12,
            ERR_NO_ERROR);  // Expect no error for the thirteenth frame

  // Process the fourteenth frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_13 =
      whole_frame_checker.payload_valid_ctrl(packet_13, current_time);
  EXPECT_EQ(valid_err_13,
            ERR_NO_ERROR);  // Expect no error for the fourteenth frame

  // Process the fifteenth frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_14 =
      whole_frame_checker.payload_valid_ctrl(packet_14, current_time);
  EXPECT_EQ(valid_err_14,
            ERR_NO_ERROR);  // Expect no error for the fifteenth frame

  // Process the sixteenth frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_15 =
      whole_frame_checker.payload_valid_ctrl(packet_15, current_time);
  EXPECT_EQ(valid_err_15,
            ERR_NO_ERROR);  // Expect no error for the sixteenth frame

  // Process the seventeenth frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_16 =
      whole_frame_checker.payload_valid_ctrl(packet_16, current_time);
  EXPECT_EQ(valid_err_16,
            ERR_NO_ERROR);  // Expect no error for the seventeenth frame

  // Process the eighteenth frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_17 =
      whole_frame_checker.payload_valid_ctrl(packet_17, current_time);
  EXPECT_EQ(valid_err_17,
            ERR_NO_ERROR);  // Expect no error for the eighteenth frame

  // Process the nineteenth frame
  current_time = std::chrono::steady_clock::now();
  uint8_t valid_err_18 =
      whole_frame_checker.payload_valid_ctrl(packet_18, current_time);
  EXPECT_EQ(valid_err_18,
            ERR_NO_ERROR);  // Expect no error for the nineteenth frame

  // Check if at least one frame is created
  ASSERT_GE(whole_frame_checker.processed_frames.size(), 1)
      << "There should be at least two frame created.";

  // Use iterator to access the first frame's frame number
  auto first_frame_number =
      whole_frame_checker.processed_frames[0]->frame_number;
  auto second_frame_number = whole_frame_checker.frames.front()
                                 ->frame_number;  // frames의 첫 번째 요소

  // Check that two different frames are processed by comparing frame numbers
  ASSERT_NE(first_frame_number, second_frame_number)
      << "The two frames should have different frame numbers.";
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
