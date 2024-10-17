#ifndef CURSES_FUNCTIONS_HPP
#define CURSES_FUNCTIONS_HPP

#include "curses/curses.h"
#include <signal.h>

// Global flag to check for exit
extern bool running;

// Struct to hold all windows
struct Windows {
    WINDOW *total_win1;
    WINDOW *win1;
    WINDOW *win2;
    WINDOW *win3;
    WINDOW *win4;
    WINDOW *win5;
    WINDOW *win6;
};

// Signal handler to stop the loop
void handle_sigint(int sig);

// Function to print and scroll when necessary
void print_and_scroll(WINDOW *win, int height, int i, int count);

// Function to initialize ncurses
void init_ncurses();

// Function to create windows and return them in a struct
struct Windows create_windows();

// Function for the main loop
void tui_loop_ctrl(struct Windows win);
void tui_interrupt_ctrl(struct Windows win);

#endif // CURSES_FUNCTIONS_HPP