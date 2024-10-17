#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include "curses/curses.h"
#include "curses/curses_functions.hpp"
#include "validuvc/uvcpheader_checker.hpp"

// Global flag to check for exit
bool running = true;

// Signal handler to stop the loop
void handle_sigint(int sig) {
    running = false;
}

// Function to print and scroll when necessary
void print_and_scroll(WINDOW *win, int height, int i, int count) {
    if (i < height - 1) {
        mvwprintw(win, i, 1, "Count: %d", count);
    } else {
        wscrl(win, 1);
        mvwprintw(win, height - 2, 1, "Count: %d", count);
    }
    wrefresh(win);
}

// Function to initialize ncurses
void init_ncurses() {
    initscr();
    nodelay(stdscr, TRUE); // Non-blocking input

    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    start_color();

    // Define color pairs
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
    init_pair(8, COLOR_BLACK, COLOR_WHITE);
    init_pair(9, COLOR_WHITE, COLOR_BLUE);
    init_pair(10, COLOR_WHITE, COLOR_CYAN);

    // Set background color for stdscr
    wbkgd(stdscr, COLOR_PAIR(7));
    refresh();
}

// Function to create windows and return them in a struct
struct Windows create_windows() {
    struct Windows win;

    int total_height = 45, total_width = 114;
    int height1 = 5, width1 = 60;
    int height2 = 5, width2 = 20;
    int height3 = 20, width3 = 81;
    int height4 = 5, width4 = 81;
    int height5 = 5, width5 = 81;
    int height6 = 43, width6 = 28;

    win.total_win1 = newwin(total_height, total_width, 1, 2);
    win.win1 = newwin(height1, width1, 2, 3);
    win.win2 = newwin(height2, width2, 2, 64);
    win.win3 = newwin(height3, width3, 8, 3);
    win.win4 = newwin(height4, width4, 29, 3);
    win.win5 = newwin(height5, width5, 35, 3);
    win.win6 = newwin(height6, width6, 2, 87);

    // Set background color for windows
    wbkgd(win.total_win1, COLOR_PAIR(8));
    wrefresh(win.total_win1);
    wbkgd(win.win1, COLOR_PAIR(7));
    wrefresh(win.win1);
    wbkgd(win.win2, COLOR_PAIR(6));
    wrefresh(win.win2);
    wbkgd(win.win3, COLOR_PAIR(3));
    wrefresh(win.win3);
    wbkgd(win.win4, COLOR_PAIR(8));
    wrefresh(win.win4);
    wbkgd(win.win5, COLOR_PAIR(8));
    wrefresh(win.win5);
    wbkgd(win.win6, COLOR_PAIR(7));
    wrefresh(win.win6);

    // Enable scrolling for windows
    scrollok(win.win1, TRUE);
    scrollok(win.win2, TRUE);
    scrollok(win.win3, TRUE);
    scrollok(win.win4, TRUE);
    scrollok(win.win5, TRUE);
    scrollok(win.win6, TRUE);

    // Print initial labels
    mvwprintw(win.win4, 1, 1, "FPS: ");
    wrefresh(win.win4);
    mvwprintw(win.win5, 1, 1, "Graph: ");
    wrefresh(win.win5);
    mvwprintw(win.win6, 1, 1, "Log: ");
    wrefresh(win.win6);

    return win;
}


void update_streaming(struct Windows win, int data) {
    static int i = 0;
    int change_data = data * 2 + i;
    
    print_and_scroll(win.win1, 5, i, change_data);
    i++;
}

void update_fps(struct Windows win, int fps) {
    static int i = 0;
    print_and_scroll(win.win1, 5, i, fps);
    i++;
}

void update_stats(struct Windows win, int data) {
    static int i = 0;
    print_and_scroll(win.win3, 5, i, data);
    i++;
}


// Function for the main loop
void tui_loop_ctrl(struct Windows win) {

    while (running) {
        int ch = getch(); // Get user input

        update_fps(win, 2);
        update_stats(win, 3);
        // update_graph

        if (ch == 'q' || ch == 'Q') {
            break;
        }

        napms(200);
    }
}

void tui_interrupt_ctrl(struct Windows win){
    // update log
    // update error
    // update stream frame
}

// int main() {
//     init_ncurses();               // Initialize ncurses
//     struct Windows win = create_windows(); // Create windows
//     tui_loop_ctrl(win);               // Run the main loop

//     endwin(); // End ncurses mode
//     return 0;
// }
