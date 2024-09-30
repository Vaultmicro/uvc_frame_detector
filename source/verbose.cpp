#include "utils/verbose.hpp"

// Initialize verbose level
int verbose_level = 1;

// VerboseStream definitions
VerboseStream v_cout_1(1, "CRUCIAL: ", std::cout);
VerboseStream v_cerr_1(1, "CRUCIAL ERROR: ", std::cerr);
VerboseStream v_cout_2(2, "BASIC: ", std::cout);
VerboseStream v_cerr_2(2, "BASIC ERROR: ", std::cerr);
VerboseStream v_cout_3(3, "DETAIL: ", std::cout);
VerboseStream v_cerr_3(3, "DETAIL ERROR: ", std::cerr);
VerboseStream v_cout_4(4, "DEV: ", std::cout);
VerboseStream v_cerr_4(4, "DEV ERROR: ", std::cerr);
VerboseStream v_cout_5(5, "ALL: ", std::cout);
VerboseStream v_cerr_5(5, "ALL ERROR: ", std::cerr);

// VerboseStream constructor implementation
VerboseStream::VerboseStream(int level, const std::string& prefix,
                             std::ostream& output_stream)
    : level_(level), prefix_(prefix), output_stream_(output_stream) {}

// VerboseStream implementation
VerboseStream& VerboseStream::operator<<(
    std::ostream& (*manip)(std::ostream&) ) {
  if (verbose_level >= level_) {
    buffer_ << manip;
    flush();
  }
  return *this;
}

void VerboseStream::flush() {
  if (verbose_level >= level_) {
    output_stream_ << buffer_.str();
    buffer_.str("");  // Clear the buffer after flushing
  }
}
