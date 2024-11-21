#include "window_manager.hpp"

WindowManager& WindowManager::getInstance() {
    static WindowManager instance;
    return instance;
}

WindowManager::WindowManager() {
    for (auto& window : windows) {
        window.counter = 1;
        window.stop_flag = false;
        window.custom_text = "";
    }
    for (auto& graph : graphs) {
        graph.stop_flag = false;
    }
}

void WindowManager::setCustomText(int index, const std::string& text) {
    if (index >= 0 && index < windows.size()) {
        std::lock_guard<std::mutex> lock(windows[index].mutex);
        windows[index].custom_text = text;
    }
}

void WindowManager::setmoveCustomText(int index, const std::string& text) {
    if (index >= 0 && index < windows.size()) {
        std::lock_guard<std::mutex> lock(windows[index].mutex);
        windows[index].custom_text = std::move(text);
    }
}

void WindowManager::addCustomText(int index, const std::string& text) {
    if (index >= 0 && index < windows.size()) {
        std::lock_guard<std::mutex> lock(windows[index].mutex);
        windows[index].custom_text += text;
    }
}

void WindowManager::addmoveCustomText(int index, const std::string& text) {
    if (index >= 0 && index < windows.size()) {
        std::lock_guard<std::mutex> lock(windows[index].mutex);
        windows[index].custom_text += std::move(text);
    }
}

void WindowManager::getCustomText(int index, std::string& text) {
    if (index >= 0 && index < windows.size()) {
        std::lock_guard<std::mutex> lock(windows[index].mutex);
        text = windows[index].custom_text;
    }
}

void WindowManager::pushbackErrorLogText(int index, const std::string& text) {
    if (index >= 0 && index < windows.size()) {
        std::lock_guard<std::mutex> lock(windows[index].mutex);
        windows[index].error_log_text.push_back(text);
    }
}

void WindowManager::pushbackSuspiciousLogText(int index, const std::string& text) {
    if (index >= 0 && index < windows.size()) {
        std::lock_guard<std::mutex> lock(windows[index].mutex);
        windows[index].suspicious_log_text.push_back(text);
    }
}

WindowData& WindowManager::getWindowData(int index) {
    return windows[index];
}

size_t WindowManager::getWindowCount() const {
    return windows.size();
}

GraphData& WindowManager::getGraphData(int index) {
    return graphs[index];
}

size_t WindowManager::getGraphCount() const {
    return graphs.size();
}
