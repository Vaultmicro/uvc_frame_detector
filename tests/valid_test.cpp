#include <gtest/gtest.h>
#include "validuvc/uvcpheader_checker.hpp"

class uvc_header_checker_test : public ::testing::Test {
protected:
    UVCPHeaderChecker header_checker;

    // Utility function to create a packet with specific data
    std::vector<u_char> create_packet(const std::vector<u_char>& data) {
        return data;
    }
};

// Test cases for valid payload
TEST_F(uvc_header_checker_test, valid_payload_test) {
    std::vector<u_char> valid_packet = create_packet({
        0x0c, 0b00001101, // HLE and BFH (valid payload)
        0x00, 0x00, 0x00, 0x01, // PTS (valid)
        0x00, 0x00, 0x00, 0x01, // SCR (valid)
    });

    uint8_t valid_err = header_checker.payload_valid_ctrl(valid_packet);
    EXPECT_EQ(valid_err, ERR_NO_ERROR);  // Expect no error
}

// Test cases for Error bit set (ERR_ERR_BIT_SET)
TEST_F(uvc_header_checker_test, err_bit_set_test) {
    std::vector<u_char> err_bit_packet = create_packet({
        0x0c, 0b01000000, // HLE and BFH with Error bit set
        0x00, 0x00, 0x00, 0x00, // PTS
        0x00, 0x00, 0x00, 0x00, // SCR
    });

    uint8_t valid_err = header_checker.payload_valid_ctrl(err_bit_packet);
    EXPECT_EQ(valid_err, ERR_ERR_BIT_SET);  // Expect ERR_ERR_BIT_SET
}

// Test cases for invalid header length (ERR_LENGTH_INVALID)
TEST_F(uvc_header_checker_test, length_out_of_range_invalid_test_small) {
    std::vector<u_char> length_invalid_packet = create_packet({
        0x00, 0b00001100, // Invalid HLE (too small)
        0x00, 0x00, 0x00, 0x00, // PTS
        0x00, 0x00, 0x00, 0x00, // SCR
    });

    uint8_t valid_err = header_checker.payload_valid_ctrl(length_invalid_packet);
    EXPECT_EQ(valid_err, ERR_LENGTH_OUT_OF_RANGE);  // Expect ERR_LENGTH_INVALID
}

// Test cases for invalid header length (ERR_LENGTH_INVALID)
TEST_F(uvc_header_checker_test, length_out_of_range_invalid_test_big) {
    std::vector<u_char> length_invalid_packet = create_packet({
        0xb4, 0b00001100, // Invalid HLE (too big)
        0x00, 0x00, 0x00, 0x00, // PTS
        0x00, 0x00, 0x00, 0x00, // SCR
    });

    uint8_t valid_err = header_checker.payload_valid_ctrl(length_invalid_packet);
    EXPECT_EQ(valid_err, ERR_LENGTH_OUT_OF_RANGE);  // Expect ERR_LENGTH_INVALID
}

// Test cases for Reserved bit set (ERR_RESERVED_BIT_SET)
TEST_F(uvc_header_checker_test, reserved_bit_set_test) {
    std::vector<u_char> reserved_bit_packet = create_packet({
        0x0c, 0b00011100, // HLE and BFH with Reserved bit set
        0x00, 0x00, 0x00, 0x01, // PTS
        0x00, 0x00, 0x00, 0x01, // SCR
    });

    uint8_t valid_err = header_checker.payload_valid_ctrl(reserved_bit_packet);
    EXPECT_EQ(valid_err, ERR_RESERVED_BIT_SET);  // Expect ERR_RESERVED_BIT_SET
}

// Test cases for Reserved bit set (ERR_RESERVED_BIT_SET)
TEST_F(uvc_header_checker_test, valid_reserved_bit_set_test) {
    std::vector<u_char> reserved_bit_packet = create_packet({
        0x0c, 0b00011110, // HLE and BFH with Reserved bit set
        0x00, 0x00, 0x00, 0x01, // PTS
        0x00, 0x00, 0x00, 0x01, // SCR
    });

    uint8_t valid_err = header_checker.payload_valid_ctrl(reserved_bit_packet);
    EXPECT_EQ(valid_err, ERR_NO_ERROR);  // Expect ERR_RESERVED_BIT_SET
}

// Test cases for Frame Identifier mismatch (ERR_FID_MISMATCH)
TEST_F(uvc_header_checker_test, fid_mismatch_test) {
    std::vector<u_char> fid_mismatch_packet = create_packet({
        0x0c, 0b00000001, // HLE and BFH with FID mismatch
        0x00, 0x00, 0x00, 0x00, // PTS
        0x00, 0x00, 0x00, 0x00, // SCR
    });

    uint8_t valid_err = header_checker.payload_valid_ctrl(fid_mismatch_packet);
    EXPECT_EQ(valid_err, ERR_FID_MISMATCH);  // Expect ERR_FID_MISMATCH
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
