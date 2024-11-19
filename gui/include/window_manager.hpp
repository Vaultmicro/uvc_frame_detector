#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#include <mutex>
#include <string>
#include <array>
#include <vector>

struct WindowData {
    int counter;
    std::string custom_text;
    std::mutex mutex;
    bool stop_flag;
    std::vector<std::string> error_log_text;
    std::vector<std::vector<std::string>> button_log_text;
    std::vector<std::string> suspicious_log_text;

    void add_error_log_text(std::string text) {
        error_log_text.push_back(text);
    }

    void add_button_log_text(){
        button_log_text.push_back(error_log_text);
        error_log_text.clear();
    }

    void add_suspicious_log_text(std::string text) {
        suspicious_log_text.push_back(text);
    }   
};

struct GraphData {
    std::mutex mutex;
    std::array<float, 10000> graph_data = {};
    int index = 0;
    std::string custom_text;
    bool stop_flag;
    std::vector<std::array<float, 10000>> error_log_graph_data;
    std::vector<std::array<float, 10000>> suspicious_log_graph_data;
    int current_graph_height = 0;
    std::vector<int> error_graph_height_history = {};
    std::vector<int> suspicious_graph_height_history = {};

    GraphData(){
        graph_data.fill(0.0f);
    }

    void addGraphData(float new_value) {
        graph_data[index] = new_value;
        index++;
        if (index >= graph_data.size()) {
            graph_reset();
        }
        if (new_value > current_graph_height){
            current_graph_height = new_value;
        }
    }

    void addErrorGraphData(){
        error_log_graph_data.push_back(graph_data);
        error_graph_height_history.push_back(current_graph_height);
    }

    void addSuspiciousGraphData(){
        suspicious_log_graph_data.push_back(graph_data);
        suspicious_graph_height_history.push_back(current_graph_height);
    }

    void graph_reset() {
        graph_data.fill(0.0f);
        index = 0;
        current_graph_height = 0;
    }

    int check_if_last(){
        if (index >= graph_data.size() - 1) {
            return 1;
        } else {
            return 0;
        }
    }
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

    GraphData& getGraphData(int index) { return graphs[index]; }
    size_t getGraphCount() const { return graphs.size(); }

private:
    WindowManager() {
        for (auto& window : windows) {
            window.counter = 1;
            window.stop_flag = false;
            window.custom_text = "";
        }
        for (auto& graph : graphs) {
            graph.stop_flag = false;
        }
    }

    std::array<WindowData, 14> windows;
    std::array<GraphData, 1> graphs;

    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;
};

#endif // WINDOW_MANAGER_HPP
