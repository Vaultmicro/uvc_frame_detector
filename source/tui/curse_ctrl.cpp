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


#include "curse_ctrl.hpp"

bool running = true; // Global flag to check for exit

void handle_sigint(int sig) {
    running = false; // Set the flag to false to exit the loop
}

void init_scr(){
    // Initialize ncurses and color support
    initscr();
    nodelay(stdscr, TRUE);

    cbreak();
    noecho();
    
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLUE);    // Window 1 color
        init_pair(2, COLOR_WHITE, COLOR_RED);     // Window 2 color
        init_pair(3, COLOR_WHITE, COLOR_GREEN);   // Window 3 color
        init_pair(4, COLOR_WHITE, COLOR_CYAN);    // Window 4 color
        init_pair(5, COLOR_WHITE, COLOR_MAGENTA); // Window 5 color
        init_pair(6, COLOR_WHITE, COLOR_YELLOW);  // Window 6 color
        init_pair(7, COLOR_WHITE, COLOR_BLACK);   // Window 7 color
        init_pair(8, COLOR_BLACK, COLOR_WHITE);   // Window 8 color
        init_pair(9, COLOR_WHITE, COLOR_BLUE);    // Window 9 color
        init_pair(10, COLOR_WHITE, COLOR_CYAN);   // Window 10 color
        init_pair(11, COLOR_WHITE, COLOR_MAGENTA); // Window 11 color
        init_pair(12, COLOR_WHITE, COLOR_YELLOW);  // Window 12 color
    }
}

void initialize_windows(){
    wbkgd(stdscr, COLOR_PAIR(7)); // Set background color for stdscr
    refresh();

    // Initialize each window separately
    curse_out_0.initialize_window(45, 114, 1, 2, 8);  // Window 0
    curse_out_1.initialize_window(5, 60, 2, 3, 1);  // Window 1
    curse_out_2.initialize_window(5, 20, 2, 64, 2); // Window 2
    curse_out_3.initialize_window(20, 81, 8, 3, 3); // Window 3
    curse_out_4.initialize_window(5, 81, 29, 3, 4); // Window 4
    curse_out_5.initialize_window(5, 81, 35, 3, 5); // Window 5
    curse_out_6.initialize_window(43, 28, 2, 87, 7); // Window 6
}

void finish_windows(){
    // Cleanup each window
    curse_out_0.cleanup_window();
    curse_out_1.cleanup_window();
    curse_out_2.cleanup_window();
    curse_out_3.cleanup_window();
    curse_out_4.cleanup_window();
    curse_out_5.cleanup_window();

    // End ncurses mode
    endwin();
}

// int main() {

//     init_scr();
//     initialize_windows();


//     while(running){
//         int ch = getch();

//         // Output to different windows
//         curse_out_1.print_and_scroll("Output to window 1");
//         curse_out_2.print_and_scroll("Output to window 2");
//         curse_out_3.print_and_scroll("Output to window 3");

//         // Add more content to windows
//         curse_out_1.print_and_scroll("More content to window 1");
//         curse_out_2.print_and_scroll("More content to window 2");


//         Sleep(300); // Simulate delay

//         if (ch == 'q' || ch == 'Q') {
//             break;
//         }

//     }

//     finish_windows();

//     return 0;
// }
