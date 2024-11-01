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

struct GraphData {
    std::mutex mutex;
    std::array<float, 150> graph_data = {};
    int index = 0;
    std::string custom_text;
    bool stop_flag;

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
