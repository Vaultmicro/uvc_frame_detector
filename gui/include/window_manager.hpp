#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#include <mutex>
#include <string>
#include <array>

struct WindowData {
    int counter;
    std::string custom_text;
    std::mutex mutex;
    bool stop_flag;
};

class WindowManager {
public:
    static WindowManager& getInstance() {
        static WindowManager instance;
        return instance;
    }

    void setCustomText(int index, const std::string& text) {
        if (index >= 0 && index < windows.size()) {
            std::lock_guard<std::mutex> lock(windows[index].mutex);
            windows[index].custom_text = text;
        }
    }

    WindowData& getWindowData(int index) { return windows[index]; }
    size_t getWindowCount() const { return windows.size(); }

private:
    WindowManager() {
        for (auto& window : windows) {
            window.counter = 1;
            window.stop_flag = false;
            window.custom_text = "";
        }
    }

    std::array<WindowData, 6> windows;

    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;
};

#endif // WINDOW_MANAGER_HPP
