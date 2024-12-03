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

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"


#define GRAPH_PERIOD_SECOND 4
#define GRAPH_PLOTTING_NUMBER_PER_MILLISECOND 8 //125 microseconds for each plotting
#define GRAPH_DATA_SIZE (GRAPH_PERIOD_SECOND * GRAPH_PLOTTING_NUMBER_PER_MILLISECOND * 1000)

// struct WindowData {
//     std::mutex mutex;
//     int counter;
//     std::string custom_text;

//     bool stop_flag;
//     std::vector<std::string> error_log_text;
//     std::vector<std::vector<std::string>> button_log_text;
//     std::vector<std::string> suspicious_log_text;
// };

// struct GraphData {
//     std::mutex mutex;
//     std::array<float, GRAPH_DATA_SIZE> graph_data = {};
//     int graph_x_index = 0;
//     std::string custom_text;
//     bool stop_flag;
//     std::vector<std::array<float, GRAPH_DATA_SIZE>> error_log_graph_data;
//     std::vector<std::array<float, GRAPH_DATA_SIZE>> suspicious_log_graph_data;
//     int max_graph_height = 0;
//     int min_graph_height = 2000000000;
//     int all_graph_height = 0;
//     int count_non_zero_graph = 0;
//     std::vector<std::array<int, 4>> error_graph_height_history = {};
//     std::vector<std::array<int, 4>> suspicious_graph_height_history = {};

//     GraphData(){
//         graph_data.fill(0.0f);
//     }
// };


class WindowData {
public:
    WindowData() : counter(0), stop_flag(false) {}


    // Public members
    void set_customtext(const std::string& text) {
        std::lock_guard<std::mutex> lock(mutex);
        custom_text = text;
    }
    void set_move_customtext(std::string&& text) {
        std::lock_guard<std::mutex> lock(mutex);
        custom_text = std::move(text);
    }
    void add_customtext(const std::string& text) {
        std::lock_guard<std::mutex> lock(mutex);
        custom_text += text;
    }
    void add_move_customtext(std::string&& text) {
        std::lock_guard<std::mutex> lock(mutex);
        custom_text += std::move(text);
    }

    void set_e3plog(const std::vector<std::vector<std::string>>& vector_text) {
        std::lock_guard<std::mutex> lock(mutex);
        e3p_log_text = vector_text;
    }
    void pushback_errorlog(const std::string& text) {
        std::lock_guard<std::mutex> lock(mutex);
        error_log_text.push_back(text);
    }
    void pushback_suspiciouslog(const std::string& text) {
        std::lock_guard<std::mutex> lock(mutex);
        suspicious_log_text.push_back(text);
    }
    void pushback_e3plog() {
        std::lock_guard<std::mutex> lock(mutex);
        e3p_log_text.push_back(error_log_text);
        error_log_text.clear();
    }


    size_t errorlogtext_size() {
        std::lock_guard<std::mutex> lock(mutex);
        return error_log_text.size();
    }
    size_t suspiciouslogtext_size() {
        std::lock_guard<std::mutex> lock(mutex);
        return suspicious_log_text.size();
    }
    size_t e3plogtext_size() {
        std::lock_guard<std::mutex> lock(mutex);
        return e3p_log_text.size();
    }
    size_t e3plogtext_frame_size(int selected_error_frame) {
        std::lock_guard<std::mutex> lock(mutex);
        return e3p_log_text[selected_error_frame].size();
    }
    
    std::vector<std::vector<std::string>> get_e3plogtext(){
        std::lock_guard<std::mutex> lock(mutex);
        return e3p_log_text;
    }

    void print_customtext() {
        std::lock_guard<std::mutex> lock(mutex);
        ImGui::Text("%s", custom_text.c_str());
    }
    void print_errorlogtext(int index) {
        std::lock_guard<std::mutex> lock(mutex);
        ImGui::Text("%s", error_log_text[index].c_str());
    }
    void print_suspiciouslogtext(int index) {
        std::lock_guard<std::mutex> lock(mutex);
        ImGui::Text("%s", suspicious_log_text[index].c_str());
    }
    void print_e3p_logtext(int error_frame_index, int error_payload_index) {
        std::lock_guard<std::mutex> lock(mutex);
        ImGui::Text("%s", e3p_log_text[error_frame_index][error_payload_index].c_str());
    }

private:
    std::mutex mutex;
    int counter;
    std::string custom_text;

    bool stop_flag;
    std::vector<std::string> error_log_text;
    std::vector<std::vector<std::string>> e3p_log_text;
    std::vector<std::string> suspicious_log_text;
};

class WindowManager {
public:
    static WindowManager& getInstance() {
        static WindowManager instance;
        return instance;
    }

    // Singleton to avoid copy and assignment
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;

    WindowData& getWin_ValidFrame () { return ValidFrameData; }
    WindowData& getWin_ErrorFrame() { return ErrorFrameData; }
    WindowData& getWin_FrameTime () { return ErrorFrameTimeData; }
    WindowData& getWin_Summary () { return SummaryData; }
    WindowData& getWin_PreviousValid () { return PreviousValidData; }
    WindowData& getWin_LostInbetweenError () { return LostInbetweenErrorData; }
    WindowData& getWin_CurrentError () { return CurrentErrorData; }
    WindowData& getWin_ControlConfig () { return ControlConfigData; }
    WindowData& getWin_Statistics () { return StatisticsData; }
    WindowData& getWin_Debug () { return DebugData; }
    WindowData& getWin_LogButtons () { return LogButtonsData; }

    WindowData& getDummyData() { return DummyData; }

    // Initial Position
    const ImVec2 ValidFrame_initial_position = ImVec2(0, 690);
    const ImVec2 ErrorFrame_initial_position = ImVec2(0, 330);
    const ImVec2 ErrorFrameTime_initial_position = ImVec2(480, 330);
    const ImVec2 Summary_initial_position = ImVec2(960, 330);
    const ImVec2 PreviousValid_initial_position = ImVec2(1440, 330);
    const ImVec2 LostInbetweenError_initial_position = ImVec2(1600, 690);
    const ImVec2 CurrentError_initial_position = ImVec2(1760, 330);
    const ImVec2 ControlConfig_initial_position = ImVec2(480, 690);
    const ImVec2 Statistics_initial_position = ImVec2(800, 690);
    const ImVec2 Debug_initial_position = ImVec2(1120, 690);
    const ImVec2 LogButtons_initial_position = ImVec2(1440, 690);
    // Window Size
    const ImVec2 ValidFrame_window_size = ImVec2(480, 360);
    const ImVec2 ErrorFrame_window_size = ImVec2(480, 360);
    const ImVec2 ErrorFrameTime_window_size = ImVec2(480, 360);
    const ImVec2 Summary_window_size = ImVec2(480, 360);
    const ImVec2 PreviousValid_window_size = ImVec2(160, 360);
    const ImVec2 LostInbetweenError_window_size = ImVec2(160, 360);
    const ImVec2 CurrentError_window_size = ImVec2(160, 360);
    const ImVec2 ControlConfig_window_size = ImVec2(320, 360);
    const ImVec2 Statistics_window_size = ImVec2(320, 360);
    const ImVec2 Debug_window_size = ImVec2(320, 360);
    const ImVec2 LogButtons_window_size = ImVec2(480, 360);

private:
    WindowManager();
    ~WindowManager();

    WindowData ValidFrameData;
    WindowData ErrorFrameData;
    WindowData ErrorFrameTimeData;
    WindowData SummaryData;
    WindowData PreviousValidData;
    WindowData LostInbetweenErrorData;
    WindowData CurrentErrorData;
    WindowData ControlConfigData;
    WindowData StatisticsData;
    WindowData DebugData;
    WindowData LogButtonsData;

    WindowData DummyData;

    std::mutex WindowManagermutex;

};


class GraphData {
public:

    GraphData() : graph_x_index(0), custom_text(""), stop_flag(false),
                  max_graph_height(0), min_graph_height(2000000000),
                  all_graph_height(0), count_non_zero_graph(0) {
        graph_data.fill(0.0f);
    }

    void update_graph_data(int index, float value) {
        if (index < 0 || index >= GRAPH_DATA_SIZE) return;

        std::lock_guard<std::mutex> lock(mutex);
        graph_data[index] = value;
    }

    void set_graph_custom_text(const std::string& text){
        std::lock_guard<std::mutex> lock(mutex);
        custom_text = text;
    }
    void set_move_graph_custom_text(std::string&& text){
        std::lock_guard<std::mutex> lock(mutex);
        custom_text = std::move(text);
    }
    
    void set_graph_data(int x, int y){
        std::lock_guard<std::mutex> lock(mutex);
        graph_data[x] = y;
        graph_x_index++;
        if (graph_x_index >= GRAPH_DATA_SIZE) {
            _reset_graph();
        }
        _update_graph_stats(y);
    }
    void add_graph_data(int new_value) {
        std::lock_guard<std::mutex> lock(mutex);
        graph_data[graph_x_index] = new_value;
        graph_x_index ++;
        if (graph_x_index >= GRAPH_DATA_SIZE) {
            _reset_graph();
        }
        _update_graph_stats(new_value);
    }
    void add_error_log_graph() {
        std::lock_guard<std::mutex> lock(mutex);
        error_log_graph_data.push_back(graph_data);
        error_graph_height_history.push_back(
            {max_graph_height, min_graph_height, all_graph_height, count_non_zero_graph});
    }
    void add_suspicious_log_graph() {
        std::lock_guard<std::mutex> lock(mutex);
        suspicious_log_graph_data.push_back(graph_data);
        suspicious_graph_height_history.push_back(
            {max_graph_height, min_graph_height, all_graph_height, count_non_zero_graph});
    }
    void reset_graph() {
        std::lock_guard<std::mutex> lock(mutex);
        _reset_graph();
    }

    int get_graph_current_x_index() {
        std::lock_guard<std::mutex> lock(mutex);
        return graph_x_index;
    }
    bool is_last_index() {
        std::lock_guard<std::mutex> lock(mutex);
        return graph_x_index >= GRAPH_DATA_SIZE - 1;
    }

private:
    void _update_graph_stats(int value) {
        if (value != 0.0f) {
            if (value > max_graph_height) max_graph_height = value;
            if (value < min_graph_height) min_graph_height = value;
            all_graph_height += value;
            count_non_zero_graph++;
        }
    }

    void _reset_graph(){
        graph_data.fill(0.0f);
        graph_x_index = 0;
        max_graph_height = 0;
        min_graph_height = 2000000000;
        all_graph_height = 0;
        count_non_zero_graph = 0;
    }

    std::mutex mutex;
    std::array<int, GRAPH_DATA_SIZE> graph_data;
    int graph_x_index;
    std::string custom_text;
    bool stop_flag;
    std::vector<std::array<int, GRAPH_DATA_SIZE>> error_log_graph_data;
    std::vector<std::array<int, GRAPH_DATA_SIZE>> suspicious_log_graph_data;

    int max_graph_height;
    int min_graph_height;
    int all_graph_height;
    int count_non_zero_graph;

    std::vector<std::array<int, 4>> error_graph_height_history;
    std::vector<std::array<int, 4>> suspicious_graph_height_history;

    const ImVec2 initial_positions_for_both = ImVec2(0, 0);
    const ImVec2 window_sizes_graph_for_both = ImVec2(1920, 330);
};

class GraphManager{
public:
    static GraphManager& getInstance() {
        static GraphManager instance;
        return instance;
    }

    // Singleton to avoid copy and assignment
    GraphManager(const GraphManager&) = delete;
    GraphManager& operator=(const GraphManager&) = delete;


    GraphData& getWin_URBGraph() { return URBTimeGraphData; }
    GraphData& getWin_PTSGraph() { return PTSTimeGraphData; }

private:
    GraphManager();
    ~GraphManager();

    GraphData URBTimeGraphData;
    GraphData PTSTimeGraphData;

    std::mutex GraphManagermutex;


};

// class WindowManager {
// public:
//     static WindowManager& getInstance();

//     // Window Data
//     void setCustomText(int index, const std::string& text);
//     void setmoveCustomText(int index, const std::string& text);
//     void addCustomText(int index, const std::string& text);
//     void addmoveCustomText(int index, const std::string& text);
//     void setButtonLogText(int index, const std::vector<std::vector<std::string>>& vector_text);
    
//     void pushbackErrorLogText(int index, const std::string& text);
//     void pushbackSuspiciousLogText(int index, const std::string& text);
//     void pushbackButtonLogText(int index);

//     const std::string& getCustomText(int index) const;
//     const std::vector<std::string>& getErrorLogText(int index) const;
//     const std::vector<std::string>& getSuspiciousLogText(int index) const;
//     const std::vector<std::vector<std::string>>& getButtonLogText(int index) const;

//     // Graph Data
//     void setGraphCustomText(int index, const std::string& text);
//     void setmoveGraphCustomText(int index, const std::string& text);

//     const int getGraphCurrentXIndex(int index);

 
//     void setGraphData(int index, int x, int y);
//     void addGraphData(int index, float new_value);
//     void addErrorGraphData(int index);
//     void addSuspiciousGraphData(int index);
//     void graph_reset(int index);
//     int check_if_last(int index);


//     std::mutex& getMutex(int index);
//     std::mutex& getGraphMutex(int index);

//     // WindowData& getWindowData(int index);
//     size_t getWindowCount() const;
//     GraphData& getGraphData(int index);
//     size_t getGraphCount() const;

// private:
//     WindowManager();

//     std::array<WindowData, 14> windows;
//     std::array<GraphData, 2> graphs;

//     WindowManager(const WindowManager&) = delete;
//     WindowManager& operator=(const WindowManager&) = delete;
// };

#endif // WINDOW_MANAGER_HPP
