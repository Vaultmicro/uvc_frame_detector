#include "gui/window_manager.hpp"

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


void WindowManager::setGraphCustomText(int index, const std::string& text) {
    if (index >= 0 && index < graphs.size()) {
        std::lock_guard<std::mutex> lock(graphs[index].mutex);
        graphs[index].custom_text = text;
    }
}

void WindowManager::setmoveGraphCustomText(int index, const std::string& text) {
    if (index >= 0 && index < graphs.size()) {
        std::lock_guard<std::mutex> lock(graphs[index].mutex);
        graphs[index].custom_text = std::move(text);
    }
}

const int WindowManager::getGraphCurrentXIndex(int index) {
    if (index >= 0 && index < graphs.size()) {
        std::lock_guard<std::mutex> lock(graphs[index].mutex);
        return graphs[index].graph_x_index;
    }
    return 0;
}



void WindowManager::addGraphData(int index, float new_value) {
    if (index >= 0 && index < graphs.size()) {
        std::lock_guard<std::mutex> lock(graphs[index].mutex);
        
        graphs[index].graph_data[graphs[index].graph_x_index] = new_value;
        graphs[index].graph_x_index++;
        if (graphs[index].graph_x_index >= graphs[index].graph_data.size()) {
            // Reset the graph data
            graphs[index].graph_data.fill(0.0f);
            graphs[index].graph_x_index = 0;
            graphs[index].max_graph_height = 0;
            graphs[index].min_graph_height = 2000000000;
            graphs[index].all_graph_height = 0;
            graphs[index].count_non_zero_graph = 0;
        }
        if (new_value != 0.0f) {
            if (new_value > graphs[index].max_graph_height) {
                graphs[index].max_graph_height = new_value;
            }
            if (new_value < graphs[index].min_graph_height) {
                graphs[index].min_graph_height = new_value;
            }
            graphs[index].all_graph_height += new_value;
            graphs[index].count_non_zero_graph++;
        }
    }
}

void WindowManager::addErrorGraphData(int index) {
    if (index >= 0 && index < graphs.size()) {
        std::lock_guard<std::mutex> lock(graphs[index].mutex);
        
        graphs[index].error_log_graph_data.push_back(graphs[index].graph_data);
        graphs[index].error_graph_height_history.push_back(std::array<int,4>{graphs[index].max_graph_height, graphs[index].min_graph_height, graphs[index].all_graph_height, graphs[index].count_non_zero_graph});
    }
}

void WindowManager::addSuspiciousGraphData(int index) {
    if (index >= 0 && index < graphs.size()) {
        std::lock_guard<std::mutex> lock(graphs[index].mutex);
        
        graphs[index].suspicious_log_graph_data.push_back(graphs[index].graph_data);
        graphs[index].suspicious_graph_height_history.push_back(std::array<int,4>{graphs[index].max_graph_height, graphs[index].min_graph_height, graphs[index].all_graph_height, graphs[index].count_non_zero_graph});
    }
}

void WindowManager::graph_reset(int index) {
    if (index >= 0 && index < graphs.size()) {
        std::lock_guard<std::mutex> lock(graphs[index].mutex);
        // Reset the graph data
        graphs[index].graph_data.fill(0.0f);
        graphs[index].graph_x_index = 0;
        graphs[index].max_graph_height = 0;
        graphs[index].min_graph_height = 2000000000;
        graphs[index].all_graph_height = 0;
        graphs[index].count_non_zero_graph = 0;
    }
}

int WindowManager::check_if_last(int index){
    if (index >= 0 && index < graphs.size()) {
        std::lock_guard<std::mutex> lock(graphs[index].mutex);
        if (graphs[index].graph_x_index >= graphs[index].graph_data.size() - 1) {
            return 1;
        } else {
            return 0;
        }
    }
    return 0;
}

std::mutex& WindowManager::getMutex(int index) {
    return windows[index].mutex;
}

std::mutex& WindowManager::getGraphMutex(int index) {
    return graphs[index].mutex;
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
