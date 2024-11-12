#ifndef VERBOSE_HPP
#define VERBOSE_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

// // Global verbose level (extern to be defined in a cpp file)
// extern int verbose_level;

// VerboseStream class for handling different verbose levels
class VerboseStream {
public:
    static int verbose_level;
    VerboseStream(int level, const std::string& prefix, std::ostream& output_stream);

    template<typename T>
    VerboseStream& operator<<(const T& message) {
        if (verbose_level >= level_) {
            buffer_ << message;
        };
        return *this;
    }

    // Overload for manipulator functions like std::endl
    VerboseStream& operator<<(std::ostream& (*manip)(std::ostream&));

    void flush();

private:
    int level_;
    std::string prefix_;
    std::ostringstream buffer_;
    std::ostream& output_stream_;
};

// Verbose log functions
void v_cout_log(const std::string& message, std::ofstream* log_file);

namespace CtrlPrint{
    extern VerboseStream v_cout_1;
    extern VerboseStream v_cerr_1;
    extern VerboseStream v_cout_2;
    extern VerboseStream v_cerr_2;
    extern VerboseStream v_cout_3;
    extern VerboseStream v_cerr_3;
    extern VerboseStream v_cout_4;
    extern VerboseStream v_cerr_4;
    extern VerboseStream v_cout_5;
    extern VerboseStream v_cerr_5;
};

#endif // VERBOSE_HPP
