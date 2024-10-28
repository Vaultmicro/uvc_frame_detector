#include <iostream>
#include <chrono>
#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "window_manager.hpp"
#include "dearimgui.hpp"

#include "other_file.hpp"

int main() {
    if (!init_imgui()) {
        return -1;
    }

    WindowManager& manager = WindowManager::getInstance();

    std::thread one_thread(one_thread_func, &manager);

    // std::array<std::thread, 6> threads;

    // threads[0] = std::thread(printing_thread_func_1, &manager.getWindowData(0));
    // threads[1] = std::thread(printing_thread_func_2, &manager.getWindowData(1));
    // threads[2] = std::thread(printing_thread_func_3, &manager.getWindowData(2));
    // threads[3] = std::thread(printing_thread_func_4, &manager.getWindowData(3));
    // threads[4] = std::thread(printing_thread_func_5, &manager.getWindowData(4));
    // threads[5] = std::thread(printing_thread_func_6, &manager.getWindowData(5));

    const ImVec2 initial_positions[6] = {
        ImVec2(0, 360), ImVec2(480, 360), ImVec2(960, 360),
        ImVec2(0, 720),   ImVec2(480, 720),  ImVec2(960, 720)
    };
    const ImVec2 window_size(480, 360);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // **Window 1 - Custom Text Print**
        {
            WindowData& data = manager.getWindowData(0);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[0], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

            ImGui::Begin("Frame Data");
            ImGui::Text("Custom Text:");
            ImGui::Text("%s", data.custom_text.c_str());
            ImGui::End();
        }

        // **Window 2 - Custom Text Print**
        {
            WindowData& data = manager.getWindowData(1);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[1], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

            ImGui::Begin("Time Data");
            ImGui::Text("Custom Text:");
            ImGui::Text("%s", data.custom_text.c_str());
            ImGui::End();
        }

        // **Window 3 - Custom Text Print**
        {
            WindowData& data = manager.getWindowData(2);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[2], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

            ImGui::Begin("Error type");
            ImGui::Text("Custom Text:");
            ImGui::Text("%s", data.custom_text.c_str());
            ImGui::End();
        }

        // **Window 4 - Counter Print**
        {
            WindowData& data = manager.getWindowData(3);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[3], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

            ImGui::Begin("Control Config");
            ImGui::Text("Counting:");
            ImGui::Text("%s", data.custom_text.c_str());
            ImGui::End();
        }

        // **Window 5 - Counter Print**
        {
            WindowData& data = manager.getWindowData(4);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[4], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

            ImGui::Begin("Statistics");
            ImGui::Text("Counting:");
            ImGui::Text("%s", data.custom_text.c_str());
            ImGui::End();
        }

        // **Window 6 - Counter Print**
        {
            WindowData& data = manager.getWindowData(5);
            std::lock_guard<std::mutex> lock(data.mutex);

            ImGui::SetNextWindowPos(initial_positions[5], ImGuiCond_Always);
            ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

            ImGui::Begin("Debug");
            ImGui::Text("Counting:");
            ImGui::Text("%s", data.custom_text.c_str());
            ImGui::End();
        }

        ImGui::Render();
        glViewport(0, 0, 1920, 1080);
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    for (int i = 0; i < 6; ++i) {
        WindowData& data = manager.getWindowData(i);
        {
            std::lock_guard<std::mutex> lock(data.mutex);
            data.stop_flag = true;
        }
    }

    if (one_thread.joinable()) {
        one_thread.join();
    }

    // for (int i = 0; i < 6; ++i) {
    //     if (threads[i].joinable()) {
    //         threads[i].join();
    //     }
    // }

    finish_imgui();
    return 0;
}
