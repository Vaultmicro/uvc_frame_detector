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

#include "gui/window_manager.hpp"

// WindowData Implementation

WindowData::WindowData() : counter(0), stop_flag(false) {}

// Setters
void WindowData::set_customtext(const std::string& text) {
    std::lock_guard<std::mutex> lock(mutex);
    custom_text = text;
}

void WindowData::set_move_customtext(std::string&& text) {
    std::lock_guard<std::mutex> lock(mutex);
    custom_text = std::move(text);
}

void WindowData::add_customtext(const std::string& text) {
    std::lock_guard<std::mutex> lock(mutex);
    custom_text += text;
}

void WindowData::add_move_customtext(std::string&& text) {
    std::lock_guard<std::mutex> lock(mutex);
    custom_text += std::move(text);
}

void WindowData::set_e3plog(const std::vector<std::vector<std::string>>& vector_text) {
    std::lock_guard<std::mutex> lock(mutex);
    e3p_log_text = vector_text;
}

void WindowData::pushback_errorlog(const std::string& text) {
    std::lock_guard<std::mutex> lock(mutex);
    error_log_text.push_back(text);
}

void WindowData::pushback_suspiciouslog(const std::string& text) {
    std::lock_guard<std::mutex> lock(mutex);
    suspicious_log_text.push_back(text);
}

void WindowData::pushback_e3plog() {
    std::lock_guard<std::mutex> lock(mutex);
    e3p_log_text.push_back(error_log_text);
    error_log_text.clear();
}

// Getters
size_t WindowData::errorlogtext_size() {
    std::lock_guard<std::mutex> lock(mutex);
    return error_log_text.size();
}

size_t WindowData::suspiciouslogtext_size() {
    std::lock_guard<std::mutex> lock(mutex);
    return suspicious_log_text.size();
}

size_t WindowData::e3plogtext_size() {
    std::lock_guard<std::mutex> lock(mutex);
    return e3p_log_text.size();
}

size_t WindowData::e3plogtext_frame_size(int selected_error_frame) {
    std::lock_guard<std::mutex> lock(mutex);
    if (selected_error_frame >= 0 && selected_error_frame < e3p_log_text.size()) {
        return e3p_log_text[selected_error_frame].size();
    }
    return 0;
}

std::vector<std::vector<std::string>> WindowData::get_e3plogtext() {
    std::lock_guard<std::mutex> lock(mutex);
    return e3p_log_text;
}

// Printing methods
void WindowData::print_customtext() {
    std::lock_guard<std::mutex> lock(mutex);
    ImGui::Text("%s", custom_text.c_str());
}

void WindowData::print_errorlogtext(int index) {
    std::lock_guard<std::mutex> lock(mutex);
    if (index >= 0 && index < error_log_text.size()) {
        ImGui::Text("%s", error_log_text[index].c_str());
    }
}

void WindowData::print_suspiciouslogtext(int index) {
    std::lock_guard<std::mutex> lock(mutex);
    if (index >= 0 && index < suspicious_log_text.size()) {
        ImGui::Text("%s", suspicious_log_text[index].c_str());
    }
}

void WindowData::print_e3p_logtext(int error_frame_index, int error_payload_index) {
    std::lock_guard<std::mutex> lock(mutex);
    if (error_frame_index >= 0 && error_frame_index < e3p_log_text.size()) {
        const auto& frame = e3p_log_text[error_frame_index];
        if (error_payload_index >= 0 && error_payload_index < frame.size()) {
            ImGui::Text("%s", frame[error_payload_index].c_str());
        }
    }
}

// WindowManager Implementation

WindowManager& WindowManager::getInstance() {
    static WindowManager instance;
    return instance;
}

WindowManager::WindowManager()
    : ValidFrame_initial_position(ImVec2(0, 690)),
      ErrorFrame_initial_position(ImVec2(0, 330)),
      ErrorFrameTime_initial_position(ImVec2(480, 330)),
      Summary_initial_position(ImVec2(960, 330)),
      PreviousValid_initial_position(ImVec2(1440, 330)),
      LostInbetweenError_initial_position(ImVec2(1600, 690)),
      CurrentError_initial_position(ImVec2(1760, 330)),
      ControlConfig_initial_position(ImVec2(480, 690)),
      Statistics_initial_position(ImVec2(800, 690)),
      Debug_initial_position(ImVec2(1120, 690)),
      LogButtons_initial_position(ImVec2(1440, 690)),
      ValidFrame_window_size(ImVec2(480, 360)),
      ErrorFrame_window_size(ImVec2(480, 360)),
      ErrorFrameTime_window_size(ImVec2(480, 360)),
      Summary_window_size(ImVec2(480, 360)),
      PreviousValid_window_size(ImVec2(160, 360)),
      LostInbetweenError_window_size(ImVec2(160, 360)),
      CurrentError_window_size(ImVec2(160, 360)),
      ControlConfig_window_size(ImVec2(320, 360)),
      Statistics_window_size(ImVec2(320, 360)),
      Debug_window_size(ImVec2(320, 360)),
      LogButtons_window_size(ImVec2(480, 360))
{
    // 추가 초기화가 필요하면 여기에 작성하세요.
}

WindowManager::~WindowManager() {
    // 필요한 정리 작업을 여기에 작성하세요.
}

// Getter Implementations
WindowData& WindowManager::getWin_ValidFrame() { return ValidFrameData; }
WindowData& WindowManager::getWin_ErrorFrame() { return ErrorFrameData; }
WindowData& WindowManager::getWin_FrameTime() { return ErrorFrameTimeData; }
WindowData& WindowManager::getWin_Summary() { return SummaryData; }
WindowData& WindowManager::getWin_PreviousValid() { return PreviousValidData; }
WindowData& WindowManager::getWin_LostInbetweenError() { return LostInbetweenErrorData; }
WindowData& WindowManager::getWin_CurrentError() { return CurrentErrorData; }
WindowData& WindowManager::getWin_ControlConfig() { return ControlConfigData; }
WindowData& WindowManager::getWin_Statistics() { return StatisticsData; }
WindowData& WindowManager::getWin_Debug() { return DebugData; }
WindowData& WindowManager::getWin_LogButtons() { return LogButtonsData; }
WindowData& WindowManager::getDummyData() { return DummyData; }

// GraphData Implementation

GraphData::GraphData()
    : graph_x_index(0), custom_text(""), stop_flag(false),
      max_graph_height(0), min_graph_height(2000000000),
      all_graph_height(0), count_non_zero_graph(0),
      initial_positions_for_both(ImVec2(0, 0)),
      window_sizes_graph_for_both(ImVec2(1920, 330))
{
    graph_data.fill(0);
}

void GraphData::update_graph_data(int index, float value) {
    if (index < 0 || index >= GRAPH_DATA_SIZE) return;
    std::lock_guard<std::mutex> lock(mutex);
    graph_data[index] = static_cast<int>(value);
}

void GraphData::set_graph_custom_text(const std::string& text) {
    std::lock_guard<std::mutex> lock(mutex);
    custom_text = text;
}

void GraphData::set_move_graph_custom_text(std::string&& text) {
    std::lock_guard<std::mutex> lock(mutex);
    custom_text = std::move(text);
}

void GraphData::set_graph_data(int x, int y) {
    std::lock_guard<std::mutex> lock(mutex);
    if (x >= 0 && x < GRAPH_DATA_SIZE) {
        graph_data[x] = y;
    }
    graph_x_index = x + 1;
    if (graph_x_index >= GRAPH_DATA_SIZE) {
        _reset_graph();
    }
    _update_graph_stats(y);
}

void GraphData::add_graph_data(int new_value) {
    std::lock_guard<std::mutex> lock(mutex);
    if (graph_x_index >= 0 && graph_x_index < GRAPH_DATA_SIZE) {
        graph_data[graph_x_index] = new_value;
        graph_x_index++;
    }
    if (graph_x_index >= GRAPH_DATA_SIZE) {
        _reset_graph();
    }
    _update_graph_stats(new_value);
}

void GraphData::add_error_log_graph() {
    std::lock_guard<std::mutex> lock(mutex);
    error_log_graph_data.push_back(graph_data);
    error_graph_height_history.push_back(
        {max_graph_height, min_graph_height, all_graph_height, count_non_zero_graph});
}

void GraphData::add_suspicious_log_graph() {
    std::lock_guard<std::mutex> lock(mutex);
    suspicious_log_graph_data.push_back(graph_data);
    suspicious_graph_height_history.push_back(
        {max_graph_height, min_graph_height, all_graph_height, count_non_zero_graph});
}

void GraphData::reset_graph() {
    std::lock_guard<std::mutex> lock(mutex);
    _reset_graph();
}

int GraphData::get_graph_current_x_index() {
    std::lock_guard<std::mutex> lock(mutex);
    return graph_x_index;
}

bool GraphData::is_last_index() {
    std::lock_guard<std::mutex> lock(mutex);
    return graph_x_index >= GRAPH_DATA_SIZE - 1;
}

// Private methods
void GraphData::_update_graph_stats(int value) {
    if (value != 0) {
        if (value > max_graph_height) max_graph_height = value;
        if (value < min_graph_height) min_graph_height = value;
        all_graph_height += value;
        count_non_zero_graph++;
    }
}

void GraphData::_reset_graph() {
    graph_data.fill(0);
    graph_x_index = 0;
    max_graph_height = 0;
    min_graph_height = 2000000000;
    all_graph_height = 0;
    count_non_zero_graph = 0;
}

// GraphManager Implementation

GraphManager& GraphManager::getInstance() {
    static GraphManager instance;
    return instance;
}

GraphManager::GraphManager() {
    // 필요한 초기화를 여기에 작성하세요.
}

GraphManager::~GraphManager() {
    // 필요한 정리 작업을 여기에 작성하세요.
}

GraphData& GraphManager::getWin_URBGraph() { return URBTimeGraphData; }
GraphData& GraphManager::getWin_PTSGraph() { return PTSTimeGraphData; }

// WindowManager& WindowManager::getInstance() {
//     static WindowManager instance;
//     return instance;
// }

// WindowManager::WindowManager() {
//     for (auto& window : windows) {
//         window.counter = 1;
//         window.stop_flag = false;
//         window.custom_text = "";
//     }
//     for (auto& graph : graphs) {
//         graph.stop_flag = false;
//     }
// }

// void WindowManager::setCustomText(int index, const std::string& text) {
//     if (index >= 0 && index < windows.size()) {
//         std::lock_guard<std::mutex> lock(windows[index].mutex);
//         windows[index].custom_text = text;
//     }
// }

// void WindowManager::setmoveCustomText(int index, const std::string& text) {
//     if (index >= 0 && index < windows.size()) {
//         std::lock_guard<std::mutex> lock(windows[index].mutex);
//         windows[index].custom_text = std::move(text);
//     }
// }

// void WindowManager::addCustomText(int index, const std::string& text) {
//     if (index >= 0 && index < windows.size()) {
//         std::lock_guard<std::mutex> lock(windows[index].mutex);
//         windows[index].custom_text += text;
//     }
// }

// void WindowManager::addmoveCustomText(int index, const std::string& text) {
//     if (index >= 0 && index < windows.size()) {
//         std::lock_guard<std::mutex> lock(windows[index].mutex);
//         windows[index].custom_text += std::move(text);
//     }
// }

// void WindowManager::setButtonLogText(int index, const std::vector<std::vector<std::string>>& vector_text) {
//     if (index >= 0 && index < windows.size()) {
//         std::lock_guard<std::mutex> lock(windows[index].mutex);
//         windows[index].button_log_text = vector_text;
//     }
// }

// void WindowManager::pushbackErrorLogText(int index, const std::string& text) {
//     if (index >= 0 && index < windows.size()) {
//         std::lock_guard<std::mutex> lock(windows[index].mutex);
//         windows[index].error_log_text.push_back(text);
//     }
// }

// void WindowManager::pushbackSuspiciousLogText(int index, const std::string& text) {
//     if (index >= 0 && index < windows.size()) {
//         std::lock_guard<std::mutex> lock(windows[index].mutex);
//         windows[index].suspicious_log_text.push_back(text);
//     }
// }

// void WindowManager::pushbackButtonLogText(int index) {
//     if (index >= 0 && index < windows.size()) {
//         std::lock_guard<std::mutex> lock(windows[index].mutex);
//         windows[index].button_log_text.push_back(windows[index].error_log_text);
//         windows[index].error_log_text.clear();
//     }
// }


// const std::string& WindowManager::getCustomText(int index) const {
//     return windows[index].custom_text;
// }

// const std::vector<std::string>& WindowManager::getErrorLogText(int index) const {
//     return windows[index].error_log_text;
// }

// const std::vector<std::string>& WindowManager::getSuspiciousLogText(int index) const {
//     return windows[index].suspicious_log_text;
// }

// const std::vector<std::vector<std::string>>& WindowManager::getButtonLogText(int index) const {
//     return windows[index].button_log_text;
// }


// void WindowManager::setGraphCustomText(int index, const std::string& text) {
//     if (index >= 0 && index < graphs.size()) {
//         std::lock_guard<std::mutex> lock(graphs[index].mutex);
//         graphs[index].custom_text = text;
//     }
// }

// void WindowManager::setmoveGraphCustomText(int index, const std::string& text) {
//     if (index >= 0 && index < graphs.size()) {
//         std::lock_guard<std::mutex> lock(graphs[index].mutex);
//         graphs[index].custom_text = std::move(text);
//     }
// }

// const int WindowManager::getGraphCurrentXIndex(int index) {
//     if (index >= 0 && index < graphs.size()) {
//         std::lock_guard<std::mutex> lock(graphs[index].mutex);
//         return graphs[index].graph_x_index;
//     }
//     return 0;
// }

// void WindowManager::setGraphData(int index, int x, int y){
//     if (index >= 0 && index < graphs.size()) {
//         std::lock_guard<std::mutex> lock(graphs[index].mutex);
        
//         graphs[index].graph_data[x] = y;
//         graphs[index].graph_x_index++;
//         if (graphs[index].graph_x_index >= graphs[index].graph_data.size()) {
//             // Reset the graph data
//             graphs[index].graph_data.fill(0.0f);
//             graphs[index].graph_x_index = 0;
//             graphs[index].max_graph_height = 0;
//             graphs[index].min_graph_height = 2000000000;
//             graphs[index].all_graph_height = 0;
//             graphs[index].count_non_zero_graph = 0;
//         }
//         if (y != 0.0f) {
//             if (y > graphs[index].max_graph_height) {
//                 graphs[index].max_graph_height = y;
//             }
//             if (y < graphs[index].min_graph_height) {
//                 graphs[index].min_graph_height = y;
//             }
//             graphs[index].all_graph_height += y;
//             graphs[index].count_non_zero_graph++;
//         }
//     }
// }

// void WindowManager::addGraphData(int index, float new_value) {
//     if (index >= 0 && index < graphs.size()) {
//         std::lock_guard<std::mutex> lock(graphs[index].mutex);
        
//         graphs[index].graph_data[graphs[index].graph_x_index] = new_value;
//         graphs[index].graph_x_index++;
//         if (graphs[index].graph_x_index >= graphs[index].graph_data.size()) {
//             // Reset the graph data
//             graphs[index].graph_data.fill(0.0f);
//             graphs[index].graph_x_index = 0;
//             graphs[index].max_graph_height = 0;
//             graphs[index].min_graph_height = 2000000000;
//             graphs[index].all_graph_height = 0;
//             graphs[index].count_non_zero_graph = 0;
//         }
//         if (new_value != 0.0f) {
//             if (new_value > graphs[index].max_graph_height) {
//                 graphs[index].max_graph_height = new_value;
//             }
//             if (new_value < graphs[index].min_graph_height) {
//                 graphs[index].min_graph_height = new_value;
//             }
//             graphs[index].all_graph_height += new_value;
//             graphs[index].count_non_zero_graph++;
//         }
//     }
// }

// void WindowManager::addErrorGraphData(int index) {
//     if (index >= 0 && index < graphs.size()) {
//         std::lock_guard<std::mutex> lock(graphs[index].mutex);
        
//         graphs[index].error_log_graph_data.push_back(graphs[index].graph_data);
//         graphs[index].error_graph_height_history.push_back(std::array<int,4>{graphs[index].max_graph_height, graphs[index].min_graph_height, graphs[index].all_graph_height, graphs[index].count_non_zero_graph});
//     }
// }

// void WindowManager::addSuspiciousGraphData(int index) {
//     if (index >= 0 && index < graphs.size()) {
//         std::lock_guard<std::mutex> lock(graphs[index].mutex);
        
//         graphs[index].suspicious_log_graph_data.push_back(graphs[index].graph_data);
//         graphs[index].suspicious_graph_height_history.push_back(std::array<int,4>{graphs[index].max_graph_height, graphs[index].min_graph_height, graphs[index].all_graph_height, graphs[index].count_non_zero_graph});
//     }
// }

// void WindowManager::graph_reset(int index) {
//     if (index >= 0 && index < graphs.size()) {
//         std::lock_guard<std::mutex> lock(graphs[index].mutex);
//         // Reset the graph data
//         graphs[index].graph_data.fill(0.0f);
//         graphs[index].graph_x_index = 0;
//         graphs[index].max_graph_height = 0;
//         graphs[index].min_graph_height = 2000000000;
//         graphs[index].all_graph_height = 0;
//         graphs[index].count_non_zero_graph = 0;
//     }
// }

// int WindowManager::check_if_last(int index){
//     if (index >= 0 && index < graphs.size()) {
//         std::lock_guard<std::mutex> lock(graphs[index].mutex);
//         if (graphs[index].graph_x_index >= graphs[index].graph_data.size() - 1) {
//             return 1;
//         } else {
//             return 0;
//         }
//     }
//     return 0;
// }

// std::mutex& WindowManager::getMutex(int index) {
//     if (index >= 0 && index < windows.size()) {
//         return windows[index].mutex;
//     }
//     static std::mutex dummy_mutex;
//     return dummy_mutex;
// }

// std::mutex& WindowManager::getGraphMutex(int index) {
//     if (index >= 0 && index < graphs.size()) {
//         return graphs[index].mutex;
//     }
//     static std::mutex dummy_mutex;
//     return dummy_mutex;
// }

// size_t WindowManager::getWindowCount() const {
//     return windows.size();
// }

// GraphData& WindowManager::getGraphData(int index) {
//     if (index >= 0 && index < graphs.size()) {
//         return graphs[index];
//     }
//     static GraphData dummy_graph;
//     return dummy_graph;
// }

// size_t WindowManager::getGraphCount() const {
//     return graphs.size();
// }
