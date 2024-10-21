#include <windows.h>
#include <iostream>

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


void setCursorPosition(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(hConsole, position);
}

void clearConsoleArea(int startX, int startY, int width, int height) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD charsWritten;

    for (int row = 0; row < height; ++row) {
        COORD coord = { static_cast<SHORT>(startX), static_cast<SHORT>(startY + row) };

        FillConsoleOutputCharacter(hConsole, ' ', width, coord, &charsWritten);

        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
        FillConsoleOutputAttribute(hConsole, consoleInfo.wAttributes, width, coord, &charsWritten);
    }

    COORD startCoord = { static_cast<SHORT>(startX), static_cast<SHORT>(startY) };
    SetConsoleCursorPosition(hConsole, startCoord);
}

void setColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void fillBackgroundColor(int startX, int startY, int width, int height, WORD backgroundColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD charsWritten;

    for (int row = 0; row < height; ++row) {
        COORD coord = { static_cast<SHORT>(startX), static_cast<SHORT>(startY + row) };
        FillConsoleOutputAttribute(hConsole, backgroundColor, width, coord, &charsWritten);
    }
}


int main() {
    // Clear the entire console window, setting width = 180 and height = 50
    clearConsoleArea(0, 0, 200, 50);

    // Set the main window size to 90x25 with gray background and black text
    fillBackgroundColor(0, 0, 100, 25, BG_WHITE); // Gray background
    setColor(BLACK); // Black text


    // First window (top-left)
    clearConsoleArea(1, 1, 48, 8);
    fillBackgroundColor(1, 1, 48, 8, BG_BLACK); // Black background
    setCursorPosition(2, 2);
    setColor(WHITE); // White text
    std::cout << "Count: 61\nCount: 62\nCount: 63\nCount: 64";

    // Second window (top-right)
    clearConsoleArea(49, 1, 48, 8);
    fillBackgroundColor(49, 1, 48, 8, BG_BLACK); // Black background
    setCursorPosition(50, 2);
    setColor(WHITE); // White text
    std::cout << "Count: 30\nCount: 40\nCount: 50\nCount: 60";

    // // Third window (bottom-left, large graph window)
    // clearConsoleArea(1, 15, 43 * 2 + 1, 11);
    // fillBackgroundColor(1, 15, 43 * 2 + 1, 11, BG_BLACK); // Black background
    // setCursorPosition(2, 16);
    // setColor(WHITE); // White text
    // std::cout << "Graph: \nFPS:";

    // // Fourth window (right-side log window)
    // clearConsoleArea(93, 1, 44, 25); // Adjusted width and height for the log window
    // fillBackgroundColor(93, 1, 44, 25, BG_BLACK); // Black background
    // setCursorPosition(94, 2);
    // setColor(WHITE); // White text
    // std::cout << "Log:";

    // Wait for user input before closing
    setCursorPosition(0, 49); // Move the cursor to the last line
    setColor(WHITE | BG_BLACK); // Reset to default colors
    std::cout << "Press any key to exit...";
    std::cin.get(); // Wait for user input


    
    clearConsoleArea(0, 0, 200, 50);

    return 0;
}
