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

#include "utils/logger.hpp"

#include <iostream>

#include "utils/verbose.hpp"

// Define the global log verbose level
int log_verbose_level = 0;

// Constructor: Initialize with log directory and file name
Logger::Logger(const std::string& log_dir_path,
               const std::string& log_file_name) {
  // Ensure the log directory exists
  ensure_log_directory(log_dir_path);

  // Create full log file path
  std::string log_path = log_dir_path + "/" + log_file_name;

  // Open log file
  if (!open_log_file(log_path)) {
    CtrlPrint::v_cerr_1 << "Failed to open log file: " + log_path
             << std::endl;  
  } else {
    CtrlPrint::v_cout_2 << "Log file created: " + log_path << std::endl; 
  }
}

// Destructor: Close log file if it's open
Logger::~Logger() {
  if (log_file.is_open()) {
    log_file.close();
  }
}

// Private helper to ensure log directory exists
void Logger::ensure_log_directory(const std::string& log_dir_path) {
  if (!fs::exists(log_dir_path)) {
    fs::create_directory(log_dir_path);
  }
}

// Private helper to open the log file
bool Logger::open_log_file(const std::string& log_path) {
  log_file.open(log_path, std::ios::out | std::ios::app);
  return log_file.is_open();
}

// General log function for all log levels
void Logger::log(int level, const std::string& level_name,
                 const std::string& message) {
  if (log_verbose_level >= level) {
    if (log_file.is_open()) {
      log_file << level_name << ": " << message << std::endl; 
    }
  }
}

// General function for logging to both console and file
void Logger::cout_log(int level, const std::string& level_name,
                      const std::string& message) {
  if (log_verbose_level >= level) {
    switch (level) {
      case 1:
        CtrlPrint::v_cout_1 << level_name + ": " + message << std::endl;
        break; 
      case 2:
        CtrlPrint::v_cout_2 << level_name + ": " + message << std::endl;
        break; 
      case 3:
        CtrlPrint::v_cout_3 << level_name + ": " + message << std::endl;
        break; 
      case 4:
        CtrlPrint::v_cout_4 << level_name + ": " + message << std::endl;
        break; 
      case 5:
        CtrlPrint::v_cout_5 << level_name + ": " + message << std::endl;
        break;  
      default:
        break;
    }
    if (log_file.is_open()) {
      log_file << level_name << ": " << message << std::endl;
    }
  }
}


// Log level 1: Crucial messages 
void Logger::log_1(const std::string& message) { log(1, "CRUCIAL", message); }

// Log level 2: Basic messages 
void Logger::log_2(const std::string& message) { log(2, "BASIC", message); }

// Log level 3: Detail messages 
void Logger::log_3(const std::string& message) { log(3, "DETAIL", message); }

// Log level 4: Dev messages 
void Logger::log_4(const std::string& message) { log(4, "DEV", message); }

// Log level 5: All messages 
void Logger::log_5(const std::string& message) { log(5, "ALL", message); }

// Log level 1: Crucial messages 
void Logger::cout_log_1(const std::string& message) {
  cout_log(1, "CRUCIAL", message);
}

// Log level 2: Basic messages 
void Logger::cout_log_2(const std::string& message) {
  cout_log(2, "BASIC", message);
}

// Log level 3: Detail messages 
void Logger::cout_log_3(const std::string& message) {
  cout_log(3, "DETAIL", message);
}

// Log level 4: Dev messages 
void Logger::cout_log_4(const std::string& message) {
  cout_log(4, "DEV", message);
}

// Log level 5: All messages 
void Logger::cout_log_5(const std::string& message) {
  cout_log(5, "ALL", message);
}
