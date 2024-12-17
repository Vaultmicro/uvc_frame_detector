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
#include "validuvc/control_config.hpp"

// Constants for scales
#define GRAPH_PERIOD_MILLISECOND 2000                   //if want to see non compressed graph, change this value to 250 and turn off the GRAPH_RESCALE to 1                   
#define GRAPH_PLOTTING_NUMBER_PER_MILLISECOND 8         //8 for 125 microseconds for each plotting
#define GRAPH_DATA_SIZE (GRAPH_PERIOD_MILLISECOND * GRAPH_PLOTTING_NUMBER_PER_MILLISECOND)
#define GRAPH_RESCALE ((GRAPH_PERIOD_MILLISECOND*GRAPH_PLOTTING_NUMBER_PER_MILLISECOND)/1600) // 1600 is the factor for 1920x1080 resolution that no graph data is being cropped
//                                                                                               // Change only if the resolution is changed (1600 should be smaller than 1920)
// #define GRAPH_RESCALE 1
#define GRAPH_NEW_SIZE (GRAPH_DATA_SIZE/GRAPH_RESCALE)

// This for just showing graph scale
#define GRAPH_SCALE_INTERVAL_MILLISECOND 200        //200 milliseconds for each scale, if 30 fps, 6 frames for each scale

class WindowData {
public:
    WindowData(const ImVec2& initial_pos, const ImVec2& window_sz);

    // Producer interface
    void set_customtext(const std::string& text);
    void set_move_customtext(std::string&& text);
    void add_customtext(const std::string& text);
    void add_move_customtext(std::string&& text);

    void set_e3plog(const std::vector<std::vector<std::string>>& vector_text);
    void pushback_errorlog(const std::string& text);
    void pushback_suspiciouslog(const std::string& text);
    void pushback_e3plog();

    // Consumer interface
    const ImVec2& get_initial_position() const;
    const ImVec2& get_window_size() const;

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

    const ImVec2 initial_position;
    const ImVec2 window_size;
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
    WindowData& getWin_GraphWindow();

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
    WindowData GraphWindowData;

    std::mutex WindowManagermutex;
};


class GraphData {
public:
    GraphData(const std::string& graph_box_nm, const ImVec2& graph_box_sz, const ImVec4& graph_box_color, const int self_type);

    // Producer interface
        //update
    void update_graph_data(int index, float value);         //not used
    void set_graph_custom_text(const std::string& text);
    void set_move_graph_custom_text(std::string&& text);
    void count_sof();
        //save history
    void add_error_log_graph();
    void add_suspicious_log_graph();
    void reset_reference_timepoint();
        //calculation for the graph plotting
    void plot_graph(std::chrono::time_point<std::chrono::steady_clock> current_time, int y);
    void compressed_in_payload_unit(std::chrono::time_point<std::chrono::steady_clock> current_time, int y);
        //scale
    void set_mark_scale();

    // Consumer interface
    auto get_error_log_graph_data_size() -> size_t;
    auto get_suspicious_log_graph_data_size() -> size_t;
    auto get_graph_current_x_index() -> int;
    bool is_last_index();

    void update_max_graph_height_of_all_time();
    void show_stream_info();
    void show_error_log_info(int selected_error_frame);
    void show_suspicious_log_info(int selected_suspicious_frame);
    void show_current_graph_data();
    void show_error_graph_data(int selected_error_frame);
    void show_suspicious_graph_data(int selected_suspicious_frame);

private:
    std::mutex mutex;

    // Graph Info
    int max_graph_height;
    int min_graph_height;
    int all_graph_height;
    int count_non_zero_graph;
    int frame_count;
    std::vector<std::array<int, 5>> error_graph_height_history;
    std::vector<std::array<int, 5>> suspicious_graph_height_history;
    int max_graph_height_of_all_time;
    int self_type;

    // Box Info
    std::string graph_box_name;
    ImVec2 graph_box_size;
    ImVec4 graph_box_color;

    // Drawing Data
    std::array<float, GRAPH_DATA_SIZE> graph_data;
    int graph_x_index;
    std::string custom_text;
    bool stop_flag;
    std::vector<std::array<float, GRAPH_DATA_SIZE>> error_log_graph_data;
    std::vector<std::array<float, GRAPH_DATA_SIZE>> suspicious_log_graph_data;

    // Calculation for the graph plotting
    std::chrono::time_point<std::chrono::steady_clock> current_time;
    std::chrono::time_point<std::chrono::steady_clock> reference_timepoint;
    std::chrono::milliseconds::rep time_gap;
    
    // Private methods : do not use mutex inside
    int new_max_graph_height_of_all_time;
    std::array<float, GRAPH_NEW_SIZE> new_graph_data;
    void resize_graph_scale_(std::array<float, GRAPH_DATA_SIZE>& prev_graph_data);

    void update_graph_stats_(int value);
    void reset_graph_();
    void add_graph_data_(int new_value);

    void init_current_time_(std::chrono::time_point<std::chrono::steady_clock> current_time);
    void calculate_time_gap_();
    void calculate_pts_overflow_();
    void update_switch_();
    void draw_graph_(int y);
    void draw_scale_();

    // value for same graph pattern shape for every graph
    static int counting_for_compression;
    std::array<std::chrono::time_point<std::chrono::steady_clock>, GRAPH_RESCALE> temp_time;
    std::array<int, 5> temp_y;
};

class GraphManager {
public:
    static GraphManager& getInstance();

    GraphData& getGraph_URBGraph();
    GraphData& getGraph_SOFGraph();
    GraphData& getGraph_TMKScale();
    GraphData& getGraph_PTSGraph();

private:
    GraphManager();
    ~GraphManager();

    GraphData URBTimeGraphData;
    GraphData ScaleSOFData;
    GraphData TickMarkScaleData;
    GraphData PTSTimeGraphData;

    std::mutex GraphManagermutex;
};


#endif // WINDOW_MANAGER_HPP
