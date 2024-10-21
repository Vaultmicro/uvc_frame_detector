#ifndef CURSED_OUTPUT_HPP
#define CURSED_OUTPUT_HPP

#include "curses.h"
#include <string>
#include <vector>

// CursedStream class for managing window-specific output
class CursedStream {
public:
    CursedStream(int window_num);
    
    template<typename T>
    CursedStream& operator<<(const T& message) {
        if (window_ != nullptr) {
            wprintw(window_, "%s", std::to_string(message).c_str());
            wrefresh(window_);
        }
        return *this;
    }

    CursedStream& operator<<(const char* message) {
        if (window_ != nullptr) {
            wprintw(window_, "%s", message);
            wrefresh(window_);
        }
        return *this;
    }

    // Overload for manipulator functions like std::endl
    CursedStream& operator<<(std::ostream& (*manip)(std::ostream&));

    void initialize_window(int height, int width, int start_y, int start_x, int color_pair);     // Initialize an individual window with color
    void cleanup_window();    // Cleanup the individual window
    void print_and_scroll(const char* message);  

private:
    int window_num_;
    WINDOW* window_ = nullptr; // Pointer to individual window
    int scroll_index_ = 0;
    int height_;
    int width_;
};

// Global extern objects for different windows
extern CursedStream curse_out_0;
extern CursedStream curse_out_1;
extern CursedStream curse_out_2;
extern CursedStream curse_out_3;
extern CursedStream curse_out_4;
extern CursedStream curse_out_5;
extern CursedStream curse_out_6;


#endif // CURSED_OUTPUT_HPP
