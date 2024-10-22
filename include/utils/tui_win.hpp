#ifndef TUI_WIN_HPP
#define TUI_WIN_HPP

#include <windows.h>
#include <iostream>
#include <csignal>
#include <sstream>
#include <vector>
#include <string>

#define BLACK       0
#define BLUE        FOREGROUND_BLUE
#define GREEN       FOREGROUND_GREEN
#define CYAN        (FOREGROUND_GREEN | FOREGROUND_BLUE)
#define RED         FOREGROUND_RED
#define PURPLE      (FOREGROUND_RED | FOREGROUND_BLUE)
#define YELLOW      (FOREGROUND_RED | FOREGROUND_GREEN)
#define WHITE       (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define BRIGHT      FOREGROUND_INTENSITY // Bright color flag

#define BG_BLACK    0
#define BG_BLUE     BACKGROUND_BLUE
#define BG_GREEN    BACKGROUND_GREEN
#define BG_CYAN     (BACKGROUND_GREEN | BACKGROUND_BLUE)
#define BG_RED      BACKGROUND_RED
#define BG_PURPLE   (BACKGROUND_RED | BACKGROUND_BLUE)
#define BG_YELLOW   (BACKGROUND_RED | BACKGROUND_GREEN)
#define BG_WHITE    (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)
#define BG_BRIGHT   BACKGROUND_INTENSITY // Bright background flag

// Structure to hold window configuration
struct WindowConfig {
    int startX;
    int startY;
    int width;
    int height;
    WORD backgroundColor;
};

// Extern declaration for window configuration vector
extern std::vector<WindowConfig> windowConfigs;
extern int window_number;

// Function declarations
void setCursorPosition(int x, int y);
void clearConsoleArea(int startX, int startY, int width, int height);
void setColor(WORD color);
void fillBackgroundColor(int startX, int startY, int width, int height, WORD backgroundColor);
void handle_sigint(int sig);
void set_console_size(int width, int height);
void print_scroll(int windowNumber, const std::string& newData);
void print_whole(int windowNumber, const std::string& newData);
void setupWindows();
void tui();

#endif // TUI_WIN_HPP
