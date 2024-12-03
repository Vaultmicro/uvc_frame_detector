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
