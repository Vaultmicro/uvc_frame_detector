#include "utils/logger.hpp"
#include "utils/verbose.hpp"
#include <iostream>

// Define the global log verbose level
int log_verbose_level = 0;
extern int verbose_level;

// Constructor: Initialize with log directory and file name
Logger::Logger(const std::string& log_dir_path, const std::string& log_file_name) {
    // Ensure the log directory exists
    ensure_log_directory(log_dir_path);

    // Create full log file path
    std::string log_path = log_dir_path + "/" + log_file_name;

    // Open log file
    if (!open_log_file(log_path)) {
        v_cerr_1 << "Failed to open log file: " + log_path << std::endl;  // 수정된 부분
    } else {
        v_cout_2 << "Log file created: " + log_path << std::endl;  // 수정된 부분
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
void Logger::log(int level, const std::string& level_name, const std::string& message) {
    if (log_verbose_level >= level) {
        if (log_file.is_open()) {
            log_file << level_name << ": " << message << std::endl;  // 파일에 기록
        }
    }
}

// General function for logging to both console and file
void Logger::cout_log(int level, const std::string& level_name, const std::string& message) {
    if (log_verbose_level >= level) {
        switch (level) {
            case 1: v_cout_1 << level_name + ": " + message << std::endl; break;  // 수정된 부분
            case 2: v_cout_2 << level_name + ": " + message << std::endl; break;  // 수정된 부분
            case 3: v_cout_3 << level_name + ": " + message << std::endl; break;  // 수정된 부분
            case 4: v_cout_4 << level_name + ": " + message << std::endl; break;  // 수정된 부분
            case 5: v_cout_5 << level_name + ": " + message << std::endl; break;  // 수정된 부분
            default: break;
        }
        // 파일 기록
        if (log_file.is_open()) {
            log_file << level_name << ": " << message << std::endl;
        }
    }
}

// 각각의 로그 레벨 함수는 더 이상 중복된 코드를 사용하지 않습니다.

// Log level 1: Crucial messages (파일에만 기록)
void Logger::log_1(const std::string& message) {
    log(1, "CRUCIAL", message);
}

// Log level 2: Basic messages (파일에만 기록)
void Logger::log_2(const std::string& message) {
    log(2, "BASIC", message);
}

// Log level 3: Detail messages (파일에만 기록)
void Logger::log_3(const std::string& message) {
    log(3, "DETAIL", message);
}

// Log level 4: Dev messages (파일에만 기록)
void Logger::log_4(const std::string& message) {
    log(4, "DEV", message);
}

// Log level 5: All messages (파일에만 기록)
void Logger::log_5(const std::string& message) {
    log(5, "ALL", message);
}

// Log level 1: Crucial messages (콘솔 및 파일에 기록)
void Logger::cout_log_1(const std::string& message) {
    cout_log(1, "CRUCIAL", message);
}

// Log level 2: Basic messages (콘솔 및 파일에 기록)
void Logger::cout_log_2(const std::string& message) {
    cout_log(2, "BASIC", message);
}

// Log level 3: Detail messages (콘솔 및 파일에 기록)
void Logger::cout_log_3(const std::string& message) {
    cout_log(3, "DETAIL", message);
}

// Log level 4: Dev messages (콘솔 및 파일에 기록)
void Logger::cout_log_4(const std::string& message) {
    cout_log(4, "DEV", message);
}

// Log level 5: All messages (콘솔 및 파일에 기록)
void Logger::cout_log_5(const std::string& message) {
    cout_log(5, "ALL", message);
}
