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

// Constants
#define GRAPH_PERIOD_SECOND 4
#define GRAPH_PLOTTING_NUMBER_PER_MILLISECOND 8
#define GRAPH_DATA_SIZE (GRAPH_PERIOD_SECOND * GRAPH_PLOTTING_NUMBER_PER_MILLISECOND * 1000)

class WindowData {
public:
    WindowData();

    void set_customtext(const std::string& text);
    void set_move_customtext(std::string&& text);
    void add_customtext(const std::string& text);
    void add_move_customtext(std::string&& text);

    void set_e3plog(const std::vector<std::vector<std::string>>& vector_text);
    void pushback_errorlog(const std::string& text);
    void pushback_suspiciouslog(const std::string& text);
    void pushback_e3plog();

    size_t errorlogtext_size();
    size_t suspiciouslogtext_size();
    size_t e3plogtext_size();
    size_t e3plogtext_frame_size(int selected_error_frame);

    std::vector<std::vector<std::string>> get_e3plogtext();

    void print_customtext();
    void print_errorlogtext(int index);
    void print_suspiciouslogtext(int index);
    void print_e3p_logtext(int error_frame_index, int error_payload_index);

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
    static WindowManager& getInstance();

    WindowData& getWin_ValidFrame();
    WindowData& getWin_ErrorFrame();
    WindowData& getWin_FrameTime();
    WindowData& getWin_Summary();
    WindowData& getWin_PreviousValid();
    WindowData& getWin_LostInbetweenError();
    WindowData& getWin_CurrentError();
    WindowData& getWin_ControlConfig();
    WindowData& getWin_Statistics();
    WindowData& getWin_Debug();
    WindowData& getWin_LogButtons();
    WindowData& getDummyData();

    const ImVec2 ValidFrame_initial_position;
    const ImVec2 ErrorFrame_initial_position;
    const ImVec2 ErrorFrameTime_initial_position;
    const ImVec2 Summary_initial_position;
    const ImVec2 PreviousValid_initial_position;
    const ImVec2 LostInbetweenError_initial_position;
    const ImVec2 CurrentError_initial_position;
    const ImVec2 ControlConfig_initial_position;
    const ImVec2 Statistics_initial_position;
    const ImVec2 Debug_initial_position;
    const ImVec2 LogButtons_initial_position;

    const ImVec2 ValidFrame_window_size;
    const ImVec2 ErrorFrame_window_size;
    const ImVec2 ErrorFrameTime_window_size;
    const ImVec2 Summary_window_size;
    const ImVec2 PreviousValid_window_size;
    const ImVec2 LostInbetweenError_window_size;
    const ImVec2 CurrentError_window_size;
    const ImVec2 ControlConfig_window_size;
    const ImVec2 Statistics_window_size;
    const ImVec2 Debug_window_size;
    const ImVec2 LogButtons_window_size;

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
    GraphData();

    void update_graph_data(int index, float value);
    void set_graph_custom_text(const std::string& text);
    void set_move_graph_custom_text(std::string&& text);
    void set_graph_data(int x, int y);
    void add_graph_data(int new_value);
    void add_error_log_graph();
    void add_suspicious_log_graph();
    void reset_graph();

    int get_graph_current_x_index();
    bool is_last_index();

private:
    void _update_graph_stats(int value);
    void _reset_graph();

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

    const ImVec2 initial_positions_for_both;
    const ImVec2 window_sizes_graph_for_both;
};

class GraphManager {
public:
    static GraphManager& getInstance();

    GraphData& getWin_URBGraph();
    GraphData& getWin_PTSGraph();

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
