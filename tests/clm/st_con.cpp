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

// SIGINT signal handler for Ctrl + C
void handle_sigint(int sig) {
    clearConsoleArea(0, 0, 150, 30);
    printf("\nSIGINT received. Exiting...\n");
    exit(0); // Exit the program when Ctrl + C is pressed
}

// Function to set the console window size
void set_console_size(int width, int height) {
    COORD buffer_size = {static_cast<SHORT>(width), static_cast<SHORT>(height)};
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), buffer_size); // Set buffer size

    SMALL_RECT window_size = {0, 0, static_cast<SHORT>(width - 1), static_cast<SHORT>(height - 1)};
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &window_size); // Set window size
}

void print_scroll(int startX, int startY, int width, int height, const std::string& newData, int windowNumber) {
    static std::vector<std::vector<std::string>> windowContents(6, std::vector<std::string>(height, std::string(width, ' ')));    
    static std::vector<int> currentRows(6, 0);
    int currentRow = currentRows[windowNumber];
    windowContents[windowNumber][currentRow] = std::string(width, ' '); 
    windowContents[windowNumber][currentRow].replace(0, newData.length(), newData);

    currentRows[windowNumber] = (currentRow + 1) % height;

    for (int row = 0; row < height; ++row) {
        int printRow = (currentRows[windowNumber] + row) % height;
        setCursorPosition(startX, startY + row);
        std::cout << windowContents[windowNumber][printRow];
    }
}

int main() {
    std::ostringstream oss;

    signal(SIGINT, handle_sigint); // Register SIGINT handler for Ctrl + C

    set_console_size(148, 30); // Set the console size

    // Clear the entire console window, setting width = 180 and height = 50
    clearConsoleArea(0, 0, 148, 30);

    // Set the main window size to 90x25 with gray background and black text
    fillBackgroundColor(0, 0, 148, 30, BG_WHITE); // Gray background
    setColor(BLACK); // Black text

    // First window
    clearConsoleArea(1, 1, 146, 1);
    fillBackgroundColor(1, 1, 48, 1, BG_BLACK); // Black background
    setCursorPosition(2, 1);
    setColor(WHITE); // White text
    std::cout << "UVC Camera Device:     Frame Width:     Frame Height:     FPS:     Frame Format:     Max Frame Size:     Max Transfer Size:";


    // Second window
    clearConsoleArea(1, 3, 48, 8);
    fillBackgroundColor(1, 3, 48, 8, BG_BLACK); // Black background
    setCursorPosition(1, 4);
    setColor(WHITE); // White text
    
    // std::cout << "Count: 61\nCount: 62\nCount: 63\nCount: 64";

    // Third window
    clearConsoleArea(50, 3, 48, 8);
    fillBackgroundColor(50, 3, 48, 8, BG_BLACK); // Black background
    setCursorPosition(50, 4);
    setColor(WHITE); // White text
    // std::cout << "Count: 30\nCount: 40\nCount: 50\nCount: 60";

    // Fourth window //for stats
    clearConsoleArea(99, 3, 48, 25);
    fillBackgroundColor(99, 3, 48, 25, BG_BLACK); // Black background
    setCursorPosition(99, 4);
    setColor(WHITE); // White text

    // Fifth window // for graph
    clearConsoleArea(1, 12, 97, 7);
    fillBackgroundColor(1, 12, 97, 7, BG_BLACK); // Black background
    setCursorPosition(1, 12);
    setColor(WHITE); // White text

    // Sixth window // for header logs
    clearConsoleArea(1, 20, 97, 8);
    fillBackgroundColor(1, 20, 97, 8, BG_BLACK); // Black background
    setCursorPosition(1, 21);
    setColor(WHITE); // White text

    setCursorPosition(2,29);
    setColor(BLACK | BG_WHITE);
    std::cout << "Frame 0 saved to frame_0.jpg";

    // Wait for user input before closing
    setCursorPosition(70, 29); // Move the cursor to the last line
    setColor(WHITE | BG_BLACK); // Reset to default colors
    std::cout << "Vaultmicro Usb Video Class Camera Frame Detector..   ";
    std::cout << "Press any key to exit...";
    // std::cin.get(); // Wait for user input


    setCursorPosition(1, 4);
    setColor(WHITE); // White text
    while (true){
        std::string dataWindow2 = std::to_string(rand() % 100);
        print_scroll(1, 3, 48, 8, dataWindow2, 2);
        Sleep(200);
        std::string dataWindow3 = std::to_string(rand() % 100);
        print_scroll(50, 3, 48, 8, dataWindow3, 3);
        Sleep(200);

    }
    
    clearConsoleArea(0, 0, 150, 30);

    return 0;
}
