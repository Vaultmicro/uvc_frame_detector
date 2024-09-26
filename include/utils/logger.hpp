#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class Logger {
public:
    // Constructor: Initialize with log directory and file name
    Logger(const std::string& log_dir_path, const std::string& log_file_name);
    
    // Destructor: Close log file if it's open
    ~Logger();

    // Log level 1: Crucial messages (file only)
    void log_1(const std::string& message);

    // Log level 2: Basic messages (file only)
    void log_2(const std::string& message);

    // Log level 3: Detail messages (file only)
    void log_3(const std::string& message);

    // Log level 4: Dev messages (file only)
    void log_4(const std::string& message);

    // Log level 5: All messages (file only)
    void log_5(const std::string& message);

    // Console and log combined functions
    // Log level 1: Crucial messages (console and file)
    void cout_log_1(const std::string& message);

    // Log level 2: Basic messages (console and file)
    void cout_log_2(const std::string& message);

    // Log level 3: Detail messages (console and file)
    void cout_log_3(const std::string& message);

    // Log level 4: Dev messages (console and file)
    void cout_log_4(const std::string& message);

    // Log level 5: All messages (console and file)
    void cout_log_5(const std::string& message);

private:
    std::ofstream log_file; // Log file stream

    // Private helper to open the log file
    bool open_log_file(const std::string& log_path);

    // Ensure log directory exists
    void ensure_log_directory(const std::string& log_dir_path);
};

// Global log verbose level variable
extern int log_verbose_level;

#endif // LOGGER_HPP