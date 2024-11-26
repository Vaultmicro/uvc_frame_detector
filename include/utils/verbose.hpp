/*********************************************************************
 * Copyright (c) 2024 Vaultmicro, Inc
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*********************************************************************/

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
