#ifndef VERBOSE_HPP
#define VERBOSE_HPP

#include <string>

#include <fstream>
#include <iostream>

// Global verbose level (extern to be defined in a cpp file)
extern int verbose_level;

void v_cout_log(const std::string& message, std::ofstream* log_file);

// Verbose level 1: Basic output using cout
void v_cout_1(const std::string& message);
void v_cerr_1(const std::string& message);

// Verbose level 2: Detailed output using cout
void v_cout_2(const std::string& message);
void v_cerr_2(const std::string& message);

// Verbose level 3: Error output using cout
void v_cout_3(const std::string& message);
void v_cerr_3(const std::string& message);

// Verbose level 4: Log output using cout
void v_cout_4(const std::string& message);
void v_cerr_4(const std::string& message);

// Verbose level 5: All output using cout
void v_cout_5(const std::string& message);
void v_cerr_5(const std::string& message);

#endif // VERBOSE_HPP