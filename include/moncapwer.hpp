#ifndef MONCAPWER_HPP
#define MONCAPWER_HPP

#include <chrono>
#include <condition_variable>
#include <csignal>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <string_view>
#include <thread>
#include <vector>
#include <array>

#include "validuvc/control_config.hpp"
#include "validuvc/uvcpheader_checker.hpp"
#include "utils/verbose.hpp"
#include "develope_photo.hpp"

#ifdef TUI_SET
#include "utils/tui_win.hpp"
#elif GUI_SET
#include "gui_win.hpp"
#endif

extern std::mutex dev_f_image_mutex;
extern std::condition_variable dev_f_image_cv;
extern std::queue<std::vector<u_char>> dev_f_image_queue;

void clean_exit(int signum);
std::vector<std::string> split(const std::string& s, char delimiter);
std::chrono::time_point<std::chrono::steady_clock> convert_epoch_to_time_point(double frame_time_epoch);
void hex_string_to_bytes_append(const std::string& hex_str, std::vector<u_char>& out_vec);
void capture_packets();
void process_packets();
void develope_frame_image();

#endif // MONCAPWER_HPP