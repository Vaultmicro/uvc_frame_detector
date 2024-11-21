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

void WindowManager::setButtonLogText(int index, const std::vector<std::vector<std::string>>& vector_text) {
    if (index >= 0 && index < windows.size()) {
        std::lock_guard<std::mutex> lock(windows[index].mutex);
        windows[index].button_log_text = vector_text;
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

void WindowManager::pushbackButtonLogText(int index) {
    if (index >= 0 && index < windows.size()) {
        std::lock_guard<std::mutex> lock(windows[index].mutex);
        windows[index].button_log_text.push_back(windows[index].error_log_text);
        windows[index].error_log_text.clear();
    }
}

std::mutex& WindowManager::getMutex(int index) {
    return windows[index].mutex;
}

const std::string& WindowManager::getCustomText(int index) const {
    return windows[index].custom_text;
}

const std::vector<std::string>& WindowManager::getErrorLogText(int index) const {
    return windows[index].error_log_text;
}

const std::vector<std::string>& WindowManager::getSuspiciousLogText(int index) const {
    return windows[index].suspicious_log_text;
}

const std::vector<std::vector<std::string>>& WindowManager::getButtonLogText(int index) const {
    return windows[index].button_log_text;
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
