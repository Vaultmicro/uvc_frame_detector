#include <gtest/gtest.h>
#include "validuvc/uvcpheader_checker.hpp"

class UVCPHeaderCheckerTest : public ::testing::Test {
protected:
    UVCPHeaderChecker header_checker;

    // Utility function to create a packet with specific data
    std::vector<u_char> createPacket(const std::vector<u_char>& data) {
        return data;
    }
};

// Test cases for valid payload
TEST_F(UVCPHeaderCheckerTest, ValidPayloadTest) {
    std::vector<u_char> valid_packet = createPacket({
        0x0c, 0x0d, // HLE and BFH (valid payload)
        0x00, 0x00, 0x00, 0x01, // PTS (valid)
        0x00, 0x00, 0x00, 0x01, // SCR (valid)
        // Add more valid data if necessary
    });

    uint8_t valid_err = header_checker.payload_valid_ctrl(valid_packet);
    EXPECT_EQ(valid_err, ERR_NO_ERROR);  // Expect no error
}

// Test cases for Error bit set (ERR_ERR_BIT_SET)
TEST_F(UVCPHeaderCheckerTest, ErrBitSetTest) {
    std::vector<u_char> err_bit_packet = createPacket({
        0x0c, 0x80, // HLE and BFH with Error bit set
        0x00, 0x00, 0x00, 0x00, // PTS

    });

    uint8_t valid_err = header_checker.payload_valid_ctrl(err_bit_packet);
    EXPECT_EQ(valid_err, ERR_ERR_BIT_SET);  // Expect ERR_ERR_BIT_SET
}

// Test cases for invalid header length (ERR_LENGTH_INVALID)
TEST_F(UVCPHeaderCheckerTest, LengthOutOfRangeInvalidTest_0) {
    std::vector<u_char> length_invalid_packet = createPacket({
        0x01, 0x0c, // Invalid HLE (too small)
        0x00, 0x00, 0x00, 0x00, // PTS
        0x00, 0x00, 0x00, 0x00, // SCR
    });

    uint8_t valid_err = header_checker.payload_valid_ctrl(length_invalid_packet);
    EXPECT_EQ(valid_err, ERR_LENGTH_OUT_OF_RANGE);  // Expect ERR_LENGTH_INVALID
}
// Test cases for invalid header length (ERR_LENGTH_INVALID)
TEST_F(UVCPHeaderCheckerTest, LengthOutOfRangeInvalidTest_1) {
    std::vector<u_char> length_invalid_packet = createPacket({
        0xb4, 0x0c, // Invalid HLE (too big)
        0x00, 0x00, 0x00, 0x00, // PTS
        0x00, 0x00, 0x00, 0x00, // SCR
    });

    uint8_t valid_err = header_checker.payload_valid_ctrl(length_invalid_packet);
    EXPECT_EQ(valid_err, ERR_LENGTH_OUT_OF_RANGE);  // Expect ERR_LENGTH_INVALID
}


// Test cases for invalid PTS length (ERR_PTS_LENGTH_INVALID)
TEST_F(UVCPHeaderCheckerTest, PTSLengthInvalidTest) {
    std::vector<u_char> pts_length_invalid_packet = createPacket({
        0x0c, 0x0d, // HLE and BFH with PTS bit set
        0x00, 0x00, 0x00,  // Invalid PTS (less than 4 bytes)
    });

    uint8_t valid_err = header_checker.payload_valid_ctrl(pts_length_invalid_packet);
    EXPECT_EQ(valid_err, ERR_LENGTH_INVALID);  // Expect ERR_PTS_LENGTH_INVALID
}

// Test cases for invalid SCR length (ERR_SCR_LENGTH_INVALID)
TEST_F(UVCPHeaderCheckerTest, SCRLengthInvalidTest) {
    std::vector<u_char> scr_length_invalid_packet = createPacket({
        0x0c, 0x2C, // HLE and BFH with SCR bit set
        0x00, 0x00, 0x00, 0x01, // SCR
        0x00, 0x00, // Invalid SCR (less than 8 bytes)
    });

    uint8_t valid_err = header_checker.payload_valid_ctrl(scr_length_invalid_packet);
    EXPECT_EQ(valid_err, ERR_LENGTH_INVALID);  // Expect ERR_SCR_LENGTH_INVALID
}

// Test cases for Reserved bit set (ERR_RESERVED_BIT_SET)
TEST_F(UVCPHeaderCheckerTest, ReservedBitSetTest) {
    std::vector<u_char> reserved_bit_packet = createPacket({
        0x0c, 0x04, // HLE and BFH with Reserved bit set
        0x00, 0x00, 0x00, 0x00, // PTS
    });

    uint8_t valid_err = header_checker.payload_valid_ctrl(reserved_bit_packet);
    EXPECT_EQ(valid_err, ERR_RESERVED_BIT_SET);  // Expect ERR_RESERVED_BIT_SET
}

// Test cases for Frame Identifier mismatch (ERR_FID_MISMATCH)
TEST_F(UVCPHeaderCheckerTest, FIDMismatchTest) {
    std::vector<u_char> fid_mismatch_packet = createPacket({
        0x0c, 0x01, // HLE and BFH with FID mismatch
        0x00, 0x00, 0x00, 0x00, // PTS
    });

    uint8_t valid_err = header_checker.payload_valid_ctrl(fid_mismatch_packet);
    EXPECT_EQ(valid_err, ERR_FID_MISMATCH);  // Expect ERR_FID_MISMATCH
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
