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

#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#include <mutex>
#include <string>
#include <array>
#include <vector>

#define GRAPH_DATA_SIZE 10000

struct WindowData {
    std::mutex mutex;
    int counter;
    std::string custom_text;

    bool stop_flag;
    std::vector<std::string> error_log_text;
    std::vector<std::vector<std::string>> button_log_text;
    std::vector<std::string> suspicious_log_text;
};

struct GraphData {
    std::mutex mutex;
    std::array<float, GRAPH_DATA_SIZE> graph_data = {};
    int graph_x_index = 0;
    std::string custom_text;
    bool stop_flag;
    std::vector<std::array<float, GRAPH_DATA_SIZE>> error_log_graph_data;
    std::vector<std::array<float, GRAPH_DATA_SIZE>> suspicious_log_graph_data;
    int max_graph_height = 0;
    int min_graph_height = 2000000000;
    int all_graph_height = 0;
    int count_non_zero_graph = 0;
    std::vector<std::array<int, 4>> error_graph_height_history = {};
    std::vector<std::array<int, 4>> suspicious_graph_height_history = {};

    GraphData(){
        graph_data.fill(0.0f);
    }
};

class WindowManager {
public:
    static WindowManager& getInstance();

    // Window Data
    void setCustomText(int index, const std::string& text);
    void setmoveCustomText(int index, const std::string& text);
    void addCustomText(int index, const std::string& text);
    void addmoveCustomText(int index, const std::string& text);
    void setButtonLogText(int index, const std::vector<std::vector<std::string>>& vector_text);
    
    void pushbackErrorLogText(int index, const std::string& text);
    void pushbackSuspiciousLogText(int index, const std::string& text);
    void pushbackButtonLogText(int index);

    const std::string& getCustomText(int index) const;
    const std::vector<std::string>& getErrorLogText(int index) const;
    const std::vector<std::string>& getSuspiciousLogText(int index) const;
    const std::vector<std::vector<std::string>>& getButtonLogText(int index) const;

    // Graph Data
    void setGraphCustomText(int index, const std::string& text);
    void setmoveGraphCustomText(int index, const std::string& text);

    const int getGraphCurrentXIndex(int index);

 

    void addGraphData(int index, float new_value);
    void addErrorGraphData(int index);
    void addSuspiciousGraphData(int index);
    void graph_reset(int index);
    int check_if_last(int index);


    std::mutex& getMutex(int index);
    std::mutex& getGraphMutex(int index);

    // WindowData& getWindowData(int index);
    size_t getWindowCount() const;
    GraphData& getGraphData(int index);
    size_t getGraphCount() const;

private:
    WindowManager();

    std::array<WindowData, 14> windows;
    std::array<GraphData, 2> graphs;

    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;
};

#endif // WINDOW_MANAGER_HPP
