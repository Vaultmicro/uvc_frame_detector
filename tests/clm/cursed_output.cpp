#include "cursed_output.hpp"

CursedStream::CursedStream(int window_num)
    : window_num_(window_num), window_(nullptr), scroll_index_(0) {}

// Manipulator function overload (e.g., std::endl)
CursedStream& CursedStream::operator<<(std::ostream& (*manip)(std::ostream&)) {
    if (window_ != nullptr) {
        wprintw(window_, "\n");
        wrefresh(window_);
    }
    return *this;
}

// Initialize an individual window with color
void CursedStream::initialize_window(int height, int width, int start_y, int start_x, int color_pair) {
    if (window_ == nullptr) {
        window_ = newwin(height, width, start_y, start_x);
        wbkgd(window_, COLOR_PAIR(color_pair)); // Set background color
        wclear(window_);
        scrollok(window_, TRUE); 
        wrefresh(window_);
        height_ = height;
        width_ = width;
    }
}

// Cleanup the individual window
void CursedStream::cleanup_window() {
    if (window_ != nullptr) {
        delwin(window_);
        window_ = nullptr;
    }
}

// Print and scroll if the window is full
void CursedStream::print_and_scroll(const char* message) {
    if (window_ != nullptr) {
        // If there's space left, print the message at current scroll index
        if (scroll_index_ < height_ - 1) {
            mvwprintw(window_, scroll_index_, 1, "%s", message);
            scroll_index_++;
        } else {
            // If window is full, scroll and print the message at the bottom
            wscrl(window_, 1);
            mvwprintw(window_, height_ - 2, 1, "%s", message);
        }
        wrefresh(window_);
    }
}


// Global CursedStream objects for different windows
CursedStream curse_out_0(0);
CursedStream curse_out_1(1);
CursedStream curse_out_2(2);
CursedStream curse_out_3(3);
CursedStream curse_out_4(4);
CursedStream curse_out_5(5);
CursedStream curse_out_6(6);
