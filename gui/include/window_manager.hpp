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
    std::vector<std::string> suspicous_log_text;

    void add_error_log_text(std::string text) {
        error_log_text.push_back(text);
    }

    void add_button_log_text(){
        button_log_text.push_back(error_log_text);
        error_log_text.clear();
    }

    void add_suspicous_log_text(std::string text) {
        suspicous_log_text.push_back(text);
    }   
};

struct GraphData {
    std::mutex mutex;
    std::array<float, 300> graph_data = {};
    int index = 0;
    std::string custom_text;
    bool stop_flag;
    std::vector<std::array<float, 300>> error_log_graph_data;
    std::vector<std::array<float, 300>> suspicous_log_graph_data;

    GraphData(){
        graph_data.fill(0.0f);
    }

    void addGraphData(float new_value) {
        graph_data[index] = new_value;
        index++;
        if (index >= graph_data.size()) {
            graph_reset();
        }
    }

    void addErrorGraphData(){
        error_log_graph_data.push_back(graph_data);
    }

    void addSuspiciousGraphData(){
        suspicous_log_graph_data.push_back(graph_data);
    }

    void graph_reset() {
        graph_data.fill(0.0f);
        index = 0;
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
