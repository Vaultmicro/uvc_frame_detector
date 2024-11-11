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

#include "utils/tui_win.hpp"
#include "validuvc/control_config.hpp"

std::vector<WindowConfig> windowConfigs = {
    {1, 1, 147, 1, BG_BLACK},  //0
    {1, 3, 72, 8, BG_BLACK},   //1
    {74, 3, 25, 8, BG_BLACK},  //2
    {100, 3, 48, 25, BG_BLACK}, //3
    {1, 12, 32, 16, BG_BLACK},  //4
    {34, 12, 32, 16, BG_BLACK},  //5
    {67, 12, 32, 16, BG_BLACK},  //6
    {1, 29, 147, 9, BG_BLACK}   //7
};

int window_number = 3;
int print_whole_flag = 0;

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
    clearConsoleArea(0, 0, 149, 40);
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

// Function to scroll and print text in a window using dynamic buffer based on window's width and height
void print_scroll(int windowNumber, const std::string& newData) {
    int width = windowConfigs[windowNumber].width;
    int height = windowConfigs[windowNumber].height;

    // Create a static buffer based on window's width and height
    static std::unique_ptr<char[]> windowBuffer[8];  // Buffer for up to 6 windows
    static int currentRow[8] = { 0 };  // Current row for each window

    // Initialize buffer if not already initialized
    if (!windowBuffer[windowNumber]) {
        windowBuffer[windowNumber] = std::make_unique<char[]>(width * height);  // Create a 1D array for the buffer
        std::fill(windowBuffer[windowNumber].get(), windowBuffer[windowNumber].get() + width * height, ' ');  // Fill with empty spaces
    }

    // Split input string into lines of appropriate width
    std::istringstream iss(newData);
    std::string line;

    while (std::getline(iss, line)) {
        // Handle wrapping if line is too long for the window's width
        while (line.length() > static_cast<size_t>(width)) {
            std::string wrappedLine = line.substr(0, width);
            line = line.substr(width);

            // Add the wrapped line to the window content (current row)
            int current = currentRow[windowNumber];
            std::copy(wrappedLine.begin(), wrappedLine.end(), windowBuffer[windowNumber].get() + current * width);
            
            currentRow[windowNumber] = (current + 1) % height;
        }

        // Add remaining line (if any) to the window content
        if (!line.empty()) {
            int current = currentRow[windowNumber];
            std::copy(line.begin(), line.end(), windowBuffer[windowNumber].get() + current * width);
            std::fill(windowBuffer[windowNumber].get() + current * width + line.length(), windowBuffer[windowNumber].get() + (current + 1) * width, ' ');  // Fill remaining space with ' '

            currentRow[windowNumber] = (current + 1) % height;
        }
    }

    // Display the contents of the window
    for (int row = 0; row < height; ++row) {
        int printRow = (currentRow[windowNumber] + row) % height;
        setCursorPosition(windowConfigs[windowNumber].startX, windowConfigs[windowNumber].startY + row);
        std::cout.write(windowBuffer[windowNumber].get() + printRow * width, width);
    }
}


// Function to clear and print the whole content in a window
void print_whole(int windowNumber, const std::string& newData) {
    int width = windowConfigs[windowNumber].width;
    int height = windowConfigs[windowNumber].height;
    int startX = windowConfigs[windowNumber].startX;
    int startY = windowConfigs[windowNumber].startY;

    // Clear the window area before printing new content
    clearConsoleArea(startX, startY, width, height);

    std::string remainingData = newData;
    int row = 0;

    while (!remainingData.empty() && row < height) {
        // Find the position of the next newline character (\n)
        size_t newlinePos = remainingData.find('\n');
        std::string line;

        // If a newline is found, extract the line up to that point
        if (newlinePos != std::string::npos) {
            line = remainingData.substr(0, newlinePos + 1); // Include the newline in the extracted line
            remainingData = remainingData.substr(newlinePos + 1); // Remove the processed part
        } else {
            // No newline found, process the entire remaining data
            line = remainingData;
            remainingData.clear();
        }

        // Print the line if it fits within the window width
        while (!line.empty()) {
            std::string partToPrint;

            if (line.length() > static_cast<size_t>(width)) {
                // If the line exceeds the window width, split it
                partToPrint = line.substr(0, width);
                line = line.substr(width);
            } else {
                // The line fits within the window
                partToPrint = line;
                line.clear();
            }

            // Set the cursor position and print the part
            setCursorPosition(startX, startY + row);
            std::cout << partToPrint;
            row++;

            // If we've reached the bottom of the window, stop
            if (row >= height) {
                break;
            }
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

    set_console_size(149, 40); // Set the console size
    clearConsoleArea(0, 0, 149, 40);
    fillBackgroundColor(0, 0, 149, 40, BG_WHITE); // Gray background
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


    setCursorPosition(2, 39);
    setColor(BLACK | BG_WHITE);
    std::cout << "Frame 0 saved to frame_0.jpg";

    // Wait for user input before closing
    setCursorPosition(68, 39); // Move the cursor to the last line
    setColor(WHITE | BG_BLACK); // Reset to default colors
    std::cout << "Vaultmicro Usb Video Class Camera Frame Detector..   ";
    std::cout << "Press ctrl+c key to exit...";

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