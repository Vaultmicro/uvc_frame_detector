#include <gtest/gtest.h>

#include <chrono>

#include "validuvc/uvcpheader_checker.hpp"
#include "validuvc/control_config.hpp"

class uvc_header_checker_test : public ::testing::Test {
 protected:
  UVCPHeaderChecker header_checker;

  // Utility function to create a packet with specific data
  std::vector<u_char> create_packet(const std::vector<u_char>& data) {
    return data;
  }
};
 
// Test cases for valid payload
TEST_F(uvc_header_checker_test, valid_payload_test_0) {
  ControlConfig::instance().set_width(1280);
  ControlConfig::instance().set_height(720);
  ControlConfig::instance().set_frame_format("mjpeg");
  ControlConfig::instance().set_fps(30);
  ControlConfig::instance().set_dwMaxPayloadTransferSize(1310720);
  ControlConfig::instance().set_dwMaxVideoFrameSize(16777216);
  
  std::vector<u_char> valid_packet = create_packet({
      0x0c, 0b00001101,                    // HLE and BFH (valid payload)
      0x00, 0x00, 0x00, 0x01,              // PTS (valid)
      0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  // SCR (valid)
  });

  auto current_time = std::chrono::steady_clock::now();
  uint8_t valid_err =
      header_checker.payload_valid_ctrl(valid_packet, current_time);
  EXPECT_EQ(valid_err, ERR_NO_ERROR);  // Expect no error
}

// Test cases for valid payload
TEST_F(uvc_header_checker_test, valid_payload_test_1) {
  std::vector<u_char> valid_packet = create_packet({
      0x02, 0b00000011,                    // HLE and BFH (valid payload)
      0x00, 0x00, 0x00, 0x00,              // PTS (valid)
      0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  // SCR (valid)
  });

  auto current_time = std::chrono::steady_clock::now();
  uint8_t valid_err =
      header_checker.payload_valid_ctrl(valid_packet, current_time);
  EXPECT_EQ(valid_err, ERR_NO_ERROR);  // Expect no error
}

// Test cases for valid payload
TEST_F(uvc_header_checker_test, valid_payload_test_2) {
  std::vector<u_char> valid_packet = create_packet({
      0x02, 0b00000010,                    // HLE and BFH (valid payload)
      0x00, 0x00, 0x00, 0x00,              // PTS (valid)
      0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  // SCR (valid)
  });

  auto current_time = std::chrono::steady_clock::now();
  uint8_t valid_err =
      header_checker.payload_valid_ctrl(valid_packet, current_time);
  EXPECT_EQ(valid_err, ERR_NO_ERROR);  // Expect no error
}

// Test cases for valid payload
TEST_F(uvc_header_checker_test, valid_payload_test_3) {
  std::vector<u_char> valid_packet = create_packet({
      0x02, 0b00000001,                    // HLE and BFH (valid payload)
      0x00, 0x00, 0x00, 0x00,              // PTS (valid)
      0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  // SCR (valid)
  });

  auto current_time = std::chrono::steady_clock::now();
  uint8_t valid_err =
      header_checker.payload_valid_ctrl(valid_packet, current_time);
  EXPECT_EQ(valid_err, ERR_NO_ERROR);  // Expect no error
}


// // Test cases for valid payload
// TEST_F(uvc_header_checker_test, valid_payload_test_3) {
//     std::vector<u_char> valid_packet = create_packet({
//         0x0c, 0b00001101, // HLE and BFH (valid payload)
//         0x00, 0x00, 0x00, 0x01, // PTS (valid)
//         0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  // SCR (valid)
//     });

//     auto current_time = std::chrono::steady_clock::now();
//     uint8_t valid_err = header_checker.payload_valid_ctrl(valid_packet,
//     current_time); EXPECT_EQ(valid_err, ERR_NO_ERROR);  // Expect no error
// }

// Test cases for Error bit set (ERR_ERR_BIT_SET)
TEST_F(uvc_header_checker_test, err_bit_set_test) {
  std::vector<u_char> err_bit_packet = create_packet({
      0x0c, 0b01000000,                    // HLE and BFH with Error bit set
      0x00, 0x00, 0x00, 0x00,              // PTS
      0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  // SCR (valid)
  });

  auto current_time = std::chrono::steady_clock::now();
  uint8_t valid_err =
      header_checker.payload_valid_ctrl(err_bit_packet, current_time);
  EXPECT_EQ(valid_err, ERR_ERR_BIT_SET);  // Expect ERR_ERR_BIT_SET
}

// Test cases for invalid header length (ERR_LENGTH_INVALID)
TEST_F(uvc_header_checker_test, length_out_of_range_invalid_test_small) {
  std::vector<u_char> length_invalid_packet = create_packet({
      0x00, 0b00001100,                    // Invalid HLE (too small)
      0x00, 0x00, 0x00, 0x00,              // PTS
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // SCR
  });

  auto current_time = std::chrono::steady_clock::now();
  uint8_t valid_err =
      header_checker.payload_valid_ctrl(length_invalid_packet, current_time);
  EXPECT_EQ(valid_err, ERR_LENGTH_OUT_OF_RANGE);  // Expect ERR_LENGTH_INVALID
}

// Test cases for invalid header length (ERR_LENGTH_INVALID)
TEST_F(uvc_header_checker_test, length_out_of_range_invalid_test_big) {
  std::vector<u_char> length_invalid_packet = create_packet({
      0xb4, 0b00001100,                    // Invalid HLE (too big)
      0x00, 0x00, 0x00, 0x00,              // PTS
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // SCR
  });

  auto current_time = std::chrono::steady_clock::now();
  uint8_t valid_err =
      header_checker.payload_valid_ctrl(length_invalid_packet, current_time);
  EXPECT_EQ(valid_err, ERR_LENGTH_OUT_OF_RANGE);  // Expect ERR_LENGTH_INVALID
}

// Test cases for Reserved bit set (ERR_RESERVED_BIT_SET)
TEST_F(uvc_header_checker_test, reserved_bit_set_test) {
  std::vector<u_char> reserved_bit_packet = create_packet({
      0x0c, 0b00011100,                    // HLE and BFH with Reserved bit set
      0x00, 0x00, 0x00, 0x00,              // PTS
      0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  // SCR (valid)
  });

  auto current_time = std::chrono::steady_clock::now();
  uint8_t valid_err =
      header_checker.payload_valid_ctrl(reserved_bit_packet, current_time);
  EXPECT_EQ(valid_err, ERR_RESERVED_BIT_SET);  // Expect ERR_RESERVED_BIT_SET
}

// Test cases for Reserved bit set (VALID_RESERVED_BIT_SET)
TEST_F(uvc_header_checker_test, valid_reserved_bit_set_test) {
  std::vector<u_char> reserved_bit_packet = create_packet({
      0x0c, 0b00011111,                    // HLE and BFH with Reserved bit set
      0x00, 0x00, 0x00, 0x00,              // PTS
      0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  // SCR (valid)
  });

  auto current_time = std::chrono::steady_clock::now();
  uint8_t valid_err =
      header_checker.payload_valid_ctrl(reserved_bit_packet, current_time);
  EXPECT_EQ(valid_err, ERR_NO_ERROR);  // Expect ERR_NO_ERROR
}

// Test cases for Reserved bit set (Length Invalid)
TEST_F(uvc_header_checker_test, header_length_diff_test) {
  std::vector<u_char> reserved_bit_packet = create_packet({
      0x02, 0b00011110,                    // HLE and BFH with Reserved bit set
      0x00, 0x00, 0x00, 0x01,              // PTS
      0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  // SCR (valid)
  });

  auto current_time = std::chrono::steady_clock::now();
  uint8_t valid_err =
      header_checker.payload_valid_ctrl(reserved_bit_packet, current_time);
  EXPECT_EQ(valid_err, ERR_LENGTH_INVALID);  // Expect ERR_NO_ERROR
}

// Test cases for Frame Identifier mismatch (Missing_eof_test)
TEST_F(uvc_header_checker_test, missing_eof_test) {
  std::vector<u_char> fid_mismatch_packet_0 = create_packet({
      0x02, 0b10000000,                    // HLE and BFH with FID mismatch
      0x00, 0x00, 0x00, 0x00,              // PTS
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // SCR
  });

  std::vector<u_char> fid_mismatch_packet_1 = create_packet({
      0x02, 0b10010011,                    // HLE and BFH with FID mismatch
      0x00, 0x00, 0x00, 0x00,              // PTS
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // SCR
  });

  auto current_time_0 = std::chrono::steady_clock::now();
  uint8_t valid_err_0 =
      header_checker.payload_valid_ctrl(fid_mismatch_packet_0, current_time_0);

  auto current_time_1 = std::chrono::steady_clock::now();
  uint8_t valid_err_1 =
      header_checker.payload_valid_ctrl(fid_mismatch_packet_1, current_time_1);

  EXPECT_EQ(valid_err_0, ERR_NO_ERROR);     // Expect ERR_FID_MISMATCH
  EXPECT_EQ(valid_err_1, ERR_MISSING_EOF);  // Expect ERR_FID_MISMATCH
}

// Test cases for valid payload
TEST_F(uvc_header_checker_test, valid_payload_test_4) {
  std::vector<u_char> valid_packet = create_packet({
      0x02, 0b00000010,                    // HLE and BFH (valid payload)
      0x00, 0x00, 0x00, 0x00,              // PTS (valid)
      0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  // SCR (valid)
  });

  auto current_time = std::chrono::steady_clock::now();
  uint8_t valid_err =
      header_checker.payload_valid_ctrl(valid_packet, current_time);
  EXPECT_EQ(valid_err, ERR_NO_ERROR);  // Expect no error
}


// Test cases for Frame Identifier mismatch (ERR_FID_MISMATCH)
TEST_F(uvc_header_checker_test, fid_mismatch_test) {

  std::vector<u_char> fid_mismatch_packet_0 = create_packet({
      0x02, 0b10000011,                    // HLE and BFH with FID mismatch
      0x00, 0x00, 0x00, 0x00,              // PTS
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // SCR
  });

  std::vector<u_char> fid_mismatch_packet_1 = create_packet({
      0x02, 0b10000001,                    // HLE and BFH with FID mismatch
      0x00, 0x00, 0x00, 0x00,              // PTS
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // SCR
  });

  auto current_time_0 = std::chrono::steady_clock::now();
  uint8_t valid_err_0 =
      header_checker.payload_valid_ctrl(fid_mismatch_packet_0, current_time_0);

  auto current_time_1 = std::chrono::steady_clock::now();
  uint8_t valid_err_1 =
      header_checker.payload_valid_ctrl(fid_mismatch_packet_1, current_time_1);

  EXPECT_EQ(valid_err_0, ERR_NO_ERROR);     // Expect ERR_FID_MISMATCH
  EXPECT_EQ(valid_err_1, ERR_FID_MISMATCH);  // Expect ERR_FID_MISMATCH
}

TEST_F(uvc_header_checker_test, empty_including_header_test) {
  std::vector<u_char> no_packet = create_packet({
  });

  auto current_time = std::chrono::steady_clock::now();
  uint8_t valid_err =
      header_checker.payload_valid_ctrl(no_packet, current_time);
  EXPECT_EQ(valid_err, ERR_EMPTY_PAYLOAD);  // Expect error
}

TEST_F(uvc_header_checker_test, max_payload_overflow_test) {
  std::vector<u_char> overflow_packet = create_packet({
      0x02, 0b10000001,                    // HLE and BFH with FID mismatch
      0x00, 0x00, 0x00, 0x00,              // PTS
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // SCR
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  });
  ControlConfig::instance().set_dwMaxPayloadTransferSize(30);

  auto current_time = std::chrono::steady_clock::now();
  uint8_t valid_err =
      header_checker.payload_valid_ctrl(overflow_packet, current_time);
  EXPECT_EQ(valid_err, ERR_MAX_PAYLAOD_OVERFLOW);  // Expect error
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

