#include "gui_win.hpp"
#include <vector>
#include <algorithm>

int gui_window_number = 5;
int print_whole_flag = 0;

int start_screen(){
    if (!init_imgui()) {
        return -1;
    }
    WindowManager& manager = WindowManager::getInstance();

    return 0;
}

void screen(){

    WindowManager& manager = WindowManager::getInstance();

    const ImVec2 initial_positions[14] = {
        ImVec2(0, 360), ImVec2(480, 360), ImVec2(960, 360),
        ImVec2(480, 720), ImVec2(800, 720), ImVec2(1120, 720),
        ImVec2(1440, 360), ImVec2(1600, 360), ImVec2(1760, 360),
        ImVec2(1440, 720), ImVec2(1680, 720), ImVec2(1440, 0),
        ImVec2(0, 0), ImVec2(0, 720)
    };

    const ImVec2 window_sizes[14] = {
        ImVec2(480, 360), ImVec2(480, 360), ImVec2(480, 360),
        ImVec2(320, 360), ImVec2(320, 360), ImVec2(320, 360),
        ImVec2(160, 360), ImVec2(160, 360), ImVec2(160, 360),
        ImVec2(240, 360), ImVec2(240, 360), ImVec2(480, 360), 
        ImVec2(480, 360), ImVec2(480, 360)
    };

    const ImVec2 initial_positions_graph[1] = {
        ImVec2(480, 0)
    };

    const ImVec2 window_sizes_graph[1] = {
        ImVec2(960, 360)
    };

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // **Window 0 - Frame Data**
        {
            WindowData& data = manager.getWindowData(0);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[0], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_sizes[0], ImGuiCond_Always);

            ImGui::Begin("Error Frame Data");
            // ImGui::Text("Current Defined Configs:");
            ImGui::Text("%s", data.custom_text.c_str());

            float current_scroll_y = ImGui::GetScrollY();
            float max_scroll_y = ImGui::GetScrollMaxY();
            if (current_scroll_y >= max_scroll_y) {
                ImGui::SetScrollHereY(1.0f);
            }

            ImGui::End();
        }

        // **Window 1 - Time Data**
        {
            WindowData& data = manager.getWindowData(1);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[1], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_sizes[1], ImGuiCond_Always);

            ImGui::Begin("Error Frame: Time & Payload Size Data");
            // ImGui::Text("Custom Text:");
            ImGui::Text("%s", data.custom_text.c_str());
            
            float current_scroll_y = ImGui::GetScrollY();
            float max_scroll_y = ImGui::GetScrollMaxY();
            if (current_scroll_y >= max_scroll_y) {
                ImGui::SetScrollHereY(1.0f);
            }

            ImGui::End();
        }

        // **Window 2 - Summary**
        {
            WindowData& data = manager.getWindowData(2);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[2], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_sizes[2], ImGuiCond_Always);

            ImGui::Begin("Summary");
            ImGui::Text("%s", data.custom_text.c_str());
            
            float current_scroll_y = ImGui::GetScrollY();
            float max_scroll_y = ImGui::GetScrollMaxY();
            if (current_scroll_y >= max_scroll_y) {
                ImGui::SetScrollHereY(1.0f);
            }

            ImGui::End();
        }

        // **Window 3 - Control**
        {
            WindowData& data = manager.getWindowData(3);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[3], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_sizes[3], ImGuiCond_Always);

            ImGui::Begin("Control Config");
            ImGui::Text("Current Defined Configs:");
            ImGui::Text("%s", data.custom_text.c_str());

            float current_scroll_y = ImGui::GetScrollY();
            float max_scroll_y = ImGui::GetScrollMaxY();
            if (current_scroll_y >= max_scroll_y) {
                ImGui::SetScrollHereY(1.0f);
            }

            ImGui::End();
        }

        // **Window 4 - Statistics**
        {
            WindowData& data = manager.getWindowData(4);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[4], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_sizes[4], ImGuiCond_Always);

            ImGui::Begin("Statistics");
            // ImGui::Text("Counting:");
            ImGui::Text("%s", data.custom_text.c_str());
            ImGui::End();
        }

        // **Window 5 - Debug**
        {
            WindowData& data = manager.getWindowData(5);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[5], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_sizes[5], ImGuiCond_Always);

            ImGui::Begin("Debug");
            ImGui::Text("%s", data.custom_text.c_str());
            
            float current_scroll_y = ImGui::GetScrollY();
            float max_scroll_y = ImGui::GetScrollMaxY();
            if (current_scroll_y >= max_scroll_y) {
                ImGui::SetScrollHereY(1.0f);
            }

            ImGui::End();
        }

        // **Window 6 - **
        {
            WindowData& data = manager.getWindowData(6);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[6], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_sizes[6], ImGuiCond_Always);

            ImGui::Begin("Previous Valid Data");
            // ImGui::Text("Counting:");
            ImGui::Text("%s", data.custom_text.c_str());
            ImGui::End();
        }

        // **Window 7 - **
        {
            WindowData& data = manager.getWindowData(7);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[7], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_sizes[7], ImGuiCond_Always);

            ImGui::Begin("Lost Inbetween Error Data");
            // ImGui::Text("Counting:");
            ImGui::Text("%s", data.custom_text.c_str());
            ImGui::End();
        }

        // **Window 8 - **
        {
            WindowData& data = manager.getWindowData(8);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[8], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_sizes[8], ImGuiCond_Always);

            ImGui::Begin("Current Error Data");
            // ImGui::Text("Counting:");
            ImGui::Text("%s", data.custom_text.c_str());
            ImGui::End();
        }

        // **Window 9 - **
        {
            WindowData& data = manager.getWindowData(9);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[9], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_sizes[9], ImGuiCond_Always);

            ImGui::Begin("FPS & Lost Frames");
            // ImGui::Text("Counting:");
            ImGui::Text("%s", data.custom_text.c_str());
            
            float current_scroll_y = ImGui::GetScrollY();
            float max_scroll_y = ImGui::GetScrollMaxY();
            if (current_scroll_y >= max_scroll_y) {
                ImGui::SetScrollHereY(1.0f);
            }

            ImGui::End();
        }

        // **Window 10 - **
        {
            WindowData& data = manager.getWindowData(10);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[10], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_sizes[10], ImGuiCond_Always);

            ImGui::Begin("Throughput");
            // ImGui::Text("Counting:");
            ImGui::Text("%s", data.custom_text.c_str());
            
            float current_scroll_y = ImGui::GetScrollY();
            float max_scroll_y = ImGui::GetScrollMaxY();
            if (current_scroll_y >= max_scroll_y) {
                ImGui::SetScrollHereY(1.0f);
            }

            ImGui::End();
        }
        
        // **Graph 0 - Histogram **
        {
            GraphData& data = manager.getGraphData(0);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions_graph[0], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_sizes_graph[0], ImGuiCond_Always);

            ImGui::Begin("Histogram");

            ImGui::Text("%s", data.custom_text.c_str());

            static float max_value = 0.0f;
            float current_max = *std::max_element(data.graph_data.begin(), data.graph_data.end());
            if (current_max > max_value) {
                max_value = current_max;
            }

            ImGui::PlotHistogram(
                "Throughput Data",
                data.graph_data.data(),
                static_cast<int>(data.graph_data.size()),
                0, nullptr,
                0.0f, max_value,  
                ImVec2(960, 300)
            );

            ImGui::End();
        }

        // **Window 11  **
        {
            WindowData& data = manager.getWindowData(11);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[11], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_sizes[11], ImGuiCond_Always);

            ImGui::Begin("Error log buttons");
            ImGui::End();
        }

        // **Window 12 - Photo **
        {
            WindowData& data = manager.getWindowData(12);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[12], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_sizes[12], ImGuiCond_Always);

            ImGui::Begin("Image");


            ImGui::End();
        }

        // **Window 13  **
        {
            WindowData& data = manager.getWindowData(13);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[13], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_sizes[13], ImGuiCond_Always);

            ImGui::Begin("Valid Frame Data");
            ImGui::Text("%s", data.custom_text.c_str());
            
            float current_scroll_y = ImGui::GetScrollY();
            float max_scroll_y = ImGui::GetScrollMaxY();
            if (current_scroll_y >= max_scroll_y) {
                ImGui::SetScrollHereY(1.0f);
            }

            ImGui::End();
        }

        ImGui::Render();
        glViewport(0, 0, 1920, 1080);
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

}

void end_screen(){
    WindowManager& manager = WindowManager::getInstance();

    for (int i = 0; i < manager.getWindowCount(); ++i) {
        WindowData& data = manager.getWindowData(i);
        std::lock_guard<std::mutex> lock(data.mutex);
        data.stop_flag = true;
    }

    for (int i = 0; i < manager.getGraphCount(); ++i) {
        GraphData& graph_data = manager.getGraphData(i);
        std::lock_guard<std::mutex> lock(graph_data.mutex);
        graph_data.stop_flag = true;
    }


    finish_imgui();

}
