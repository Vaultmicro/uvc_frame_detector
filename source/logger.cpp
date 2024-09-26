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
        v_cerr_1("Failed to open log file: " + log_path);
    } else {
        v_cout_2("Log file created: " + log_path);
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

// Log level 1: Crucial messages (파일에만 기록)
void Logger::log_1(const std::string& message) {
    if (log_verbose_level >= 1 && log_file.is_open()) {
        log_file << "CRUCIAL: " << message << std::endl;
    }
}

// Log level 2: Basic messages (파일에만 기록)
void Logger::log_2(const std::string& message) {
    if (log_verbose_level >= 2 && log_file.is_open()) {
        log_file << "BASIC: " << message << std::endl;
    }
}

// Log level 3: Detail messages (파일에만 기록)
void Logger::log_3(const std::string& message) {
    if (log_verbose_level >= 3 && log_file.is_open()) {
        log_file << "DETAIL: " << message << std::endl;
    }
}

// Log level 4: Dev messages (파일에만 기록)
void Logger::log_4(const std::string& message) {
    if (log_verbose_level >= 4 && log_file.is_open()) {
        log_file << "DEV: " << message << std::endl;
    }
}

// Log level 5: All messages (파일에만 기록)
void Logger::log_5(const std::string& message) {
    if (log_verbose_level >= 5 && log_file.is_open()) {
        log_file << "ALL: " << message << std::endl;
    }
}

// Console and log combined functions

// Log level 1: Crucial messages (콘솔 및 파일에 기록)
void Logger::cout_log_1(const std::string& message) {
    if (log_verbose_level >= 1) {
        v_cout_1("CRUCIAL: " + message);  // 콘솔 출력
        if (log_file.is_open()) {
            log_file << "CRUCIAL: " << message << std::endl;  // 파일 기록
        }
    }
}

// Log level 2: Basic messages (콘솔 및 파일에 기록)
void Logger::cout_log_2(const std::string& message) {
    if (log_verbose_level >= 2) {
        v_cout_2("BASIC: " + message);  // 콘솔 출력
        if (log_file.is_open()) {
            log_file << "BASIC: " << message << std::endl;  // 파일 기록
        }
    }
}

// Log level 3: Detail messages (콘솔 및 파일에 기록)
void Logger::cout_log_3(const std::string& message) {
    if (log_verbose_level >= 3) {
        v_cout_3("DETAIL: " + message);  // 콘솔 출력
        if (log_file.is_open()) {
            log_file << "DETAIL: " << message << std::endl;  // 파일 기록
        }
    }
}

// Log level 4: Dev messages (콘솔 및 파일에 기록)
void Logger::cout_log_4(const std::string& message) {
    if (log_verbose_level >= 4) {
        v_cout_4("DEV: " + message);  // 콘솔 출력
        if (log_file.is_open()) {
            log_file << "DEV: " << message << std::endl;  // 파일 기록
        }
    }
}

// Log level 5: All messages (콘솔 및 파일에 기록)
void Logger::cout_log_5(const std::string& message) {
    if (log_verbose_level >= 5) {
        v_cout_5("ALL: " + message);  // 콘솔 출력
        if (log_file.is_open()) {
            log_file << "ALL: " << message << std::endl;  // 파일 기록
        }
    }
}