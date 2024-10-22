#include "utils/tui_win.hpp"
#include "validuvc/control_config.hpp"

std::vector<WindowConfig> windowConfigs = {
    {1, 1, 146, 1, BG_BLACK},   // First window
    {1, 3, 48, 8, BG_BLACK},    // Second window
    {50, 3, 48, 8, BG_BLACK},   // Third window
    {99, 3, 48, 25, BG_BLACK},  // Fourth window
    {1, 12, 97, 7, BG_BLACK},   // Fifth window (graph)
    {1, 20, 97, 8, BG_BLACK}    // Sixth window (header logs)
};

int window_number = 3;

// Function to set the cursor position in the console
void setCursorPosition(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(hConsole, position);
}

// Function to clear a section of the console
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

// Function to set the color of the text in the console
void setColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// Function to fill a section of the console with a background color
void fillBackgroundColor(int startX, int startY, int width, int height, WORD backgroundColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD charsWritten;

    for (int row = 0; row < height; ++row) {
        COORD coord = { static_cast<SHORT>(startX), static_cast<SHORT>(startY + row) };
        FillConsoleOutputAttribute(hConsole, backgroundColor, width, coord, &charsWritten);
    }
}

// SIGINT signal handler (Ctrl + C)
void handle_sigint(int sig) {
    clearConsoleArea(0, 0, 150, 30);
    printf("\nSIGINT received. Exiting...\n");
    exit(0);
}

// Function to set the console window size
void set_console_size(int width, int height) {
    COORD buffer_size = { static_cast<SHORT>(width), static_cast<SHORT>(height) };
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), buffer_size);

    SMALL_RECT window_size = { 0, 0, static_cast<SHORT>(width - 1), static_cast<SHORT>(height - 1) };
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &window_size);
}

// Function to scroll and print text in a window
void print_scroll(int windowNumber, const std::string& newData) {
    static std::vector<std::vector<std::string>> windowContents(6, std::vector<std::string>(windowConfigs[windowNumber].height, std::string(windowConfigs[windowNumber].width, ' ')));
    static std::vector<int> currentRows(6, 0);

    int currentRow = currentRows[windowNumber];
    windowContents[windowNumber][currentRow] = std::string(windowConfigs[windowNumber].width, ' ');
    windowContents[windowNumber][currentRow].replace(0, newData.length(), newData);

    currentRows[windowNumber] = (currentRow + 1) % windowConfigs[windowNumber].height;

    for (int row = 0; row < windowConfigs[windowNumber].height; ++row) {
        int printRow = (currentRows[windowNumber] + row) % windowConfigs[windowNumber].height;
        setCursorPosition(windowConfigs[windowNumber].startX, windowConfigs[windowNumber].startY + row);
        std::cout << windowContents[windowNumber][printRow];
    }
}

// Function to clear and print the whole content in a window
void print_whole(int windowNumber, const std::string& newData) {
    int width = windowConfigs[windowNumber].width;
    int height = windowConfigs[windowNumber].height;
    int startX = windowConfigs[windowNumber].startX;
    int startY = windowConfigs[windowNumber].startY;

    clearConsoleArea(startX, startY, width, height);

    std::istringstream iss(newData);
    std::string line;
    int row = 0;

    while (std::getline(iss, line) && row < height) {
        while (line.length() > static_cast<size_t>(width)) {
            setCursorPosition(startX, startY + row);
            std::cout << line.substr(0, width);
            line = line.substr(width);
            row++;
            if (row >= height) break;
        }

        if (row < height) {
            setCursorPosition(startX, startY + row);
            std::cout << line;
            row++;
        }
    }
}

// Function to set up windows with the specified configurations
void setupWindows() {
    for (const auto& config : windowConfigs) {
        fillBackgroundColor(config.startX, config.startY, config.width, config.height, config.backgroundColor);
        clearConsoleArea(config.startX, config.startY, config.width, config.height);
    }
}



void tui() {
    // signal(SIGINT, handle_sigint); // Register SIGINT handler for Ctrl + C

    set_console_size(148, 30); // Set the console size
    clearConsoleArea(0, 0, 148, 30);
    fillBackgroundColor(0, 0, 148, 30, BG_WHITE); // Gray background
    setColor(BLACK); // Black text


    setupWindows(); 

    setCursorPosition (2, 1);
    setColor(WHITE);
    std::cout << "  Frame Width: " << ControlConfig::width 
          << "     Frame Height: " << ControlConfig::height 
          << "     FPS: " << ControlConfig::fps 
          << "     Frame Format: " << ControlConfig::frame_format 
          << "     Max Frame Size: " << ControlConfig::dwMaxVideoFrameSize 
          << "     Max Transfer Size: " << ControlConfig::dwMaxPayloadTransferSize 
          << std::endl;


    setCursorPosition(2, 29);
    setColor(BLACK | BG_WHITE);
    std::cout << "Frame 0 saved to frame_0.jpg";

    // Wait for user input before closing
    setCursorPosition(40, 29); // Move the cursor to the last line
    setColor(WHITE | BG_BLACK); // Reset to default colors
    std::cout << "Vaultmicro Usb Video Class Camera Frame Detector..   ";
    std::cout << "Press any key to exit...";

    // while (true) {
    //     std::ostringstream ossWindow1;
    //     ossWindow1 << "Random Data: " << rand() % 100;
    //     print_scroll(1, ossWindow1.str());
    //     Sleep(200);

    //     std::ostringstream ossWindow2;
    //     ossWindow2 << "Random Data: " << rand() % 100;
    //     print_scroll(2, ossWindow2.str());
    //     Sleep(200);

    //     std::ostringstream ossWindow3;
    //     ossWindow3 << "This is the first line." << std::endl;
    //     ossWindow3 << "This is the second line, which is long and will wrap around." << std::endl;
    //     ossWindow3 << "This is the third line." << std::endl;
    //     ossWindow3 << "Random Data: " << rand() % 100;
    //     print_whole(3, ossWindow3.str());

    // }

    // clearConsoleArea(0, 0, 150, 30);

    // return 0;
}