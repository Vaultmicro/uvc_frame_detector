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

#include "gui/gui_win.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include "validuvc/uvcpheader_checker.hpp"
#include <vector>
#include <algorithm>

static std::vector<std::string> error_frame_log_button;
void addErrorFrameLog(const std::string& efn) {
    error_frame_log_button.push_back(efn);
}
const std::vector<std::string>& getErrorFrameLog() {
    return error_frame_log_button;
}

static std::vector<std::string> suspicious_frame_log_button;
void addSuspiciousFrameLog(const std::string& efn) {
    suspicious_frame_log_button.push_back(efn);
}
const std::vector<std::string>& getSuspiciousFrameLog() {
    return suspicious_frame_log_button;
}

#ifdef _WIN32
std::string image_file_path = "images\\smpte.jpg";
#elif __linux__
std::string image_file_path = "images/smpte.jpg";
#endif

GLuint texture_id = 0;

GLuint LoadTextureFromFile(const char* filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return 0;
    }

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return texture_id;
}

void UpdateImageTexture(const std::string& path) {
    if (texture_id) {
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }
    texture_id = LoadTextureFromFile(path.c_str());
}


int start_screen(){
    if (!init_imgui()) {
        return -1;
    }
    return 0;
}

void screen(){
    static bool show_error_log = false;
    static int selected_error_frame = 0;
    static int selected_error_payload = 0;
    static bool show_image = false;
    static int selected_suspicious_frame = 0;
    static bool show_suspicious_log = false;

    static bool prev_capture_error_image_flag = false;
    static bool prev_capture_suspicious_image_flag = false;
    static bool prev_capture_valid_image_flag = false;

    static bool prev_static_const_img_filter = false;
    static bool prev_pts_decrease_filter = false;
    static bool prev_scr_stc_decrease_filter = false;

    WindowManager &uvcfd_win = WindowManager::getInstance();
    GraphManager &uvcfd_graph = GraphManager::getInstance();

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // **Window 11  **
        {
            WindowData& lb_data = uvcfd_win.getWin_LogButtons();
            ImGui::SetNextWindowPos(lb_data.get_initial_position(), ImGuiCond_Always);
            ImGui::SetNextWindowSize(lb_data.get_window_size(), ImGuiCond_Always);

            ImGui::Begin("Error log buttons", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar);

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("Type")) {
                    if (ImGui::BeginMenu("Payload Error")) {
                        ImGui::TextWrapped("Payload Error Information:");
                        ImGui::BulletText("ERR_NO_ERROR = 0: No error detected in the payload.");
                        ImGui::BulletText("ERR_EMPTY_PAYLOAD = 1: UVCPayload is empty, including the header.");
                        ImGui::BulletText("ERR_MAX_PAYLOAD_OVERFLOW = 2: Payload size exceeds maximum transfer size.");
                        ImGui::BulletText("ERR_ERR_BIT_SET = 3: BFH Error bit is set. Payload is invalid.");
                        ImGui::BulletText("ERR_LENGTH_OUT_OF_RANGE = 4: HLE (Header Length Extension) is out of the valid range (2 to 12).");
                        ImGui::BulletText("ERR_LENGTH_INVALID = 5: HLE length does not match with PTS (Presentation Time Stamp) or SCR (Source Clock Reference) settings.");
                        ImGui::BulletText("ERR_RESERVED_BIT_SET = 6: Reserved bit (RES) set to 1 when EOF (End of Frame) is 0.");
                        ImGui::BulletText("ERR_EOH_BIT = 7: -deleted logic.");
                        ImGui::BulletText("ERR_TOGGLE_BIT_OVERLAPPED = 8: -deleted logic.");
                        ImGui::BulletText("ERR_FID_MISMATCH = 9: PTS matches the previous payload with the same FID, indicating a toggle bit error.");
                        ImGui::BulletText("ERR_SWAP = 10: Payload data is swapped or corrupted.");
                        ImGui::BulletText("ERR_MISSING_EOF = 11: Missing EOF in the previous frame's payload.");
                        ImGui::BulletText("ERR_UNKNOWN = 99: Unknown error.");
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Frame Error")) {
                        ImGui::TextWrapped("Frame Error Information:");
                        ImGui::BulletText("ERR_FRAME_NO_ERROR = 0: No error detected in the payload, valid.");
                        ImGui::BulletText("ERR_FRAME_DROP = 1: FPS lower than expected, classified as dropped frames.");
                        ImGui::BulletText("ERR_FRAME_ERROR = 2: Missing EOF or payload validation error.");
                        ImGui::BulletText("ERR_FRAME_MAX_FRAME_OVERFLOW = 3: Frame size exceeds maximum defined size.");
                        ImGui::BulletText("ERR_FRAME_INVALID_YUYV_RAW_SIZE = 4: YUYV frame size mismatch.");
                        ImGui::BulletText("ERR_FRAME_SAME_DIFFERENT_PTS = 5: -deleted logic.");
                        ImGui::BulletText("ERR_FRAME_MISSING_EOF = 6: Missing EOF in the frame.");
                        ImGui::BulletText("ERR_FRAME_FID_MISMATCH = 7: FID matches previous frame while EOF is set.");
                        ImGui::BulletText("ERR_FRAME_UNKNOWN = 99: Unknown error.");
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Frame Suspicious")) {
                        ImGui::TextWrapped("Frame Suspicious Information:");
                        ImGui::BulletText("SUSPICIOUS_NO_SUSPICIOUS = 0: No suspicious behavior detected.");
                        ImGui::BulletText("SUSPICIOUS_PAYLOAD_TIME_INCONSISTENT = 1: Payload timestamps are inconsistent.");
                        ImGui::BulletText("SUSPICIOUS_FRAME_SIZE_INCONSISTENT = 2: Frame sizes are inconsistent.");
                        ImGui::BulletText("SUSPICIOUS_PAYLOAD_COUNT_INCONSISTENT = 3: Payload counts are inconsistent.");
                        ImGui::BulletText("SUSPICIOUS_PTS_DECREASE = 4: PTS decreased unexpectedly, excluding overflow.");
                        ImGui::BulletText("SUSPICIOUS_SCR_STC_DECREASE = 5: SCR STC decreased unexpectedly, excluding overflow.");
                        ImGui::BulletText("SUSPICIOUS_OVERCOMPRESSED = 6: MJPEG frame size smaller than 5% of raw data.");
                        ImGui::BulletText("SUSPICIOUS_ERROR_CHECKED = 97: It is already defined as an error.");
                        ImGui::BulletText("SUSPICIOUS_UNKNOWN = 98: Unknown suspicious behavior.");
                        ImGui::BulletText("SUSPICIOUS_UNCHECKED = 99: Suspicion is not checked.");
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Help")) {
                    if (ImGui::BeginMenu("Usage")) {
                        ImGui::TextWrapped("Error Frame Drop Down Combo Box:");
                        ImGui::BulletText("When Error Frame is created, a drop-down combo box for frame and payload is generated.");
                        ImGui::BulletText("Users can select a specific Error Frame to view its data in the following windows:");
                        ImGui::BulletText("- Error Frame Data");
                        ImGui::BulletText("- Error Frame: Time & Payload Size Data");
                        ImGui::BulletText("- Summary");
                        ImGui::BulletText("- Payload Header Info");

                        ImGui::Separator();

                        ImGui::TextWrapped("Error Payload Drop Down Combo Box:");
                        ImGui::BulletText("Shows payload header information when errors occur.");
                        ImGui::BulletText("Shows payload errors for selected frames.");

                        ImGui::Separator();

                        ImGui::TextWrapped("Suspicious Frame Drop Down Combo Box:");
                        ImGui::BulletText("Enables users to select a specific Frame to view related data in the same windows.");

                        ImGui::Separator();

                        ImGui::TextWrapped("Show Image:");
                        ImGui::BulletText("When Error Log or Suspicious Log button is pressed and a Frame is selected, it shows the saved image.");
                        ImGui::BulletText("To view saved log data, press Error Log button to switch screens.");

                        ImGui::Separator();

                        ImGui::TextWrapped("Suspicious Log:");
                        ImGui::BulletText("To view saved log data, press Suspicious Log button to switch screens.");

                        ImGui::Separator();

                        ImGui::TextWrapped("Streaming Log:");
                        ImGui::BulletText("To view currently streaming data information, press Streaming Log button to switch screens.");

                        ImGui::Separator();

                        ImGui::TextWrapped("Capture On/Off:");
                        ImGui::BulletText("Switch off all three save image buttons or recall previous options.");
                        ImGui::BulletText("Capture Error Frames.");
                        ImGui::BulletText("Capture Suspicious Frames.");
                        ImGui::BulletText("Capture Valid Frames.");
                        ImGui::BulletText("Switch off all three capture filter buttons or recall previous options.");

                        ImGui::Separator();

                        ImGui::TextWrapped("Irregular:");
                        ImGui::BulletText("Detect frame size drop below 10% for last three frames, over 95% compressed frame, or payload count is smaller than the last three frames average.");

                        ImGui::Separator();

                        ImGui::TextWrapped("PTS Decrease:");
                        ImGui::BulletText("Detect PTS field decrease, excluding overflow.");

                        ImGui::Separator();

                        ImGui::TextWrapped("SCRSTCDecrease:");
                        ImGui::BulletText("Detect SCR STC field decreases, excluding overflow.");

                        ImGui::Separator();

                        ImGui::TextWrapped("Play/Pause:");
                        ImGui::BulletText("Play button to start receiving data, Pause to discard streaming data.");

                        ImGui::Separator();

                        ImGui::TextWrapped("Quit:");
                        ImGui::BulletText("Press to exit the application.");

                        ImGui::EndMenu();
                    }

                    if (ImGui::BeginMenu("Application Info")) {
                        ImGui::Text("Used Libraries:");
                        ImGui::BulletText("libjpeg-turbo 3.0.4");
                        ImGui::BulletText("stb_image");
                        ImGui::BulletText("imgui 1.91.5");
                        ImGui::BulletText("glew 2.1.0");
                        ImGui::BulletText("glfw 3.4");

                        ImGui::Separator();
                        ImGui::Text("Versions:");
                        ImGui::BulletText("Wireshark, Tshark Version 4.4.0");
                        ImGui::BulletText("USBPcapCMD version 1.5.4.0");

                        ImGui::Separator();
                        ImGui::Text("Application Version:");
                        ImGui::BulletText("Version 0.3.5");

                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            if (!error_frame_log_button.empty()) {

                ImGui::SetCursorPos(ImVec2(9, 45));
                if (ImGui::BeginCombo(":: Select Error Frame", error_frame_log_button[selected_error_frame].c_str())) {
                    for (int n = 0; n < error_frame_log_button.size(); n++) {
                        bool is_selected = (selected_error_frame == n);
                        if (ImGui::Selectable(error_frame_log_button[n].c_str(), is_selected)) {
                            show_image = false;
                            selected_error_payload = 0;
                            selected_error_frame = n; 
                        }
                        if (is_selected) {
                            ImGui::SetItemDefaultFocus(); 
                        }
                    }
                    ImGui::EndCombo();
                }

                ImGui::SetCursorPos(ImVec2(9, 75));
                if (selected_error_frame < uvcfd_win.getWin_CurrentError().e3plogtext_size()){
                    std::string current_error_label = "Error " + std::to_string(selected_error_payload);
                    if (ImGui::BeginCombo(":: Error Payload", current_error_label.c_str())) {
                        for (size_t j = 0; j < uvcfd_win.getWin_CurrentError().e3plogtext_frame_size(selected_error_frame); j++) {
                            std::string item_label = "Error " + std::to_string(j);
                            bool is_selected = (j == selected_error_payload);

                            if (ImGui::Selectable(item_label.c_str(), is_selected)) {
                                selected_error_payload = j;
                            }

                            if (is_selected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                }

            } else {
                ImGui::SetCursorPos(ImVec2(9, 75));
                ImGui::Text("    No Error Log Available");
            }

            if (!suspicious_frame_log_button.empty()){

                ImGui::SetCursorPos(ImVec2(9, 105));
                if (ImGui::BeginCombo(":: Select Suspicious", suspicious_frame_log_button[selected_suspicious_frame].c_str())) {
                    for (int n = 0; n < suspicious_frame_log_button.size(); n++) {
                        bool is_selected = (selected_suspicious_frame == n);
                        if (ImGui::Selectable(suspicious_frame_log_button[n].c_str(), is_selected)) {
                            show_image = false;
                            selected_suspicious_frame = n; 
                        }
                        if (is_selected) {
                            ImGui::SetItemDefaultFocus(); 
                        }
                    }
                    ImGui::EndCombo();
                }

            } else {
                ImGui::SetCursorPos(ImVec2(9, 105));
                ImGui::Text("    No suspicious Log Available");
            }

            ImGui::SetCursorPos(ImVec2(27, 130));
            if (show_image) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 1.0f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.60f)); 
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.80f)); 
            }
            if (ImGui::Button(show_image ? "Show Image" : "Show Image", ImVec2(96, 40))){
                if (show_error_log){
                    if (!error_frame_log_button.empty()) {
                        show_image = !show_image;
                        if (show_image) {

                            std::string selected_log_name = error_frame_log_button[selected_error_frame];
                            size_t pos = selected_log_name.find("Frame ");
                            if (pos != std::string::npos) {
                                std::string frame_number = selected_log_name.substr(pos + 6);
            #ifdef _WIN32
                                image_file_path = "images\\frame_" + frame_number + ".jpg";
            #elif __linux__
                                image_file_path = "images/frame_" + frame_number + ".jpg";
            #endif
                            } else {
            #ifdef _WIN32
                                image_file_path = "images\\smpte.jpg";
            #elif __linux__
                                image_file_path = "images/smpte.jpg";
            #endif
                            }
                            UpdateImageTexture(image_file_path);
                        }
                    }
                } else if (show_suspicious_log) {
                    if (!suspicious_frame_log_button.empty()) {
                        show_image = !show_image;
                        if (show_image) {
                            std::string selected_log_name = suspicious_frame_log_button[selected_suspicious_frame];
                            size_t pos = selected_log_name.find("Suspicious ");
                            if (pos != std::string::npos) {
                                std::string frame_number = selected_log_name.substr(pos + 11);
            #ifdef _WIN32
                                image_file_path = "images\\frame_" + frame_number + ".jpg";
            #elif __linux__
                                image_file_path = "images/frame_" + frame_number + ".jpg";
            #endif
                            } else {
            #ifdef _WIN32
                                image_file_path = "images\\smpte.jpg";
            #elif __linux__
                                image_file_path = "images/smpte.jpg";
            #endif
                            }
                            UpdateImageTexture(image_file_path);
                        }
                    }
                }
            }
            ImGui::PopStyleColor(2);

            ImGui::SetCursorPos(ImVec2(137, 130));
            if (show_error_log) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 1.0f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.60f)); 
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.80f)); 
            } 
            if (ImGui::Button(show_error_log ? "Error Log" : "Error Log", ImVec2(96, 40))) {
                if (!error_frame_log_button.empty()) {
                    show_suspicious_log = false;
                    show_error_log = true;
                    lb_data.set_customtext("Selected Log: " + error_frame_log_button[selected_error_frame]);
                } else {
                    lb_data.set_customtext("No Error");
                }
            }
            ImGui::PopStyleColor(2);

            ImGui::SetCursorPos(ImVec2(247, 130));
            if (show_suspicious_log) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 1.0f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.60f)); 
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.80f)); 
            }
            if (ImGui::Button(show_error_log ? "Suspicious Log" : "Suspicious Log", ImVec2(96, 40))) {
                if (!suspicious_frame_log_button.empty()) {
                    show_error_log = false;
                    show_suspicious_log = true;
                    lb_data.set_customtext("Selected Log: " + suspicious_frame_log_button[selected_suspicious_frame]);
                }
            }
            ImGui::PopStyleColor(2);

            ImGui::SetCursorPos(ImVec2(357, 130));
            if (!show_error_log && !show_suspicious_log) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 1.0f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.60f)); 
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.80f)); 
            }
            if (ImGui::Button(show_error_log ? "Streaming Log" : "Streaming Log", ImVec2(96, 40))) {
                show_error_log = false;
                show_suspicious_log = false;
                show_image = false;
                lb_data.set_customtext("Streaming is shown");
            }
            ImGui::PopStyleColor(2);

            ImGui::SetCursorPos(ImVec2(27, 185));
            if (UVCPHeaderChecker::capture_image_flag) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 1.0f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.60f)); 
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.80f)); 
            }
            if (ImGui::Button(UVCPHeaderChecker::capture_image_flag ? "Capture On" : "Capture Off", ImVec2(96, 40))){
                UVCPHeaderChecker::capture_image_flag = !UVCPHeaderChecker::capture_image_flag;
                if (UVCPHeaderChecker::capture_image_flag) {
                    UVCPHeaderChecker::capture_error_flag = prev_capture_error_image_flag;
                    UVCPHeaderChecker::capture_suspicious_flag = prev_capture_suspicious_image_flag;
                    UVCPHeaderChecker::capture_valid_flag = prev_capture_valid_image_flag;
                } else {
                    prev_capture_error_image_flag = UVCPHeaderChecker::capture_error_flag;
                    prev_capture_suspicious_image_flag = UVCPHeaderChecker::capture_suspicious_flag;
                    prev_capture_valid_image_flag = UVCPHeaderChecker::capture_valid_flag;

                    UVCPHeaderChecker::capture_error_flag = false;
                    UVCPHeaderChecker::capture_suspicious_flag = false;
                    UVCPHeaderChecker::capture_valid_flag = false;
                }
            }
            ImGui::PopStyleColor(2);

            ImGui::SetCursorPos(ImVec2(137, 185));
            if (UVCPHeaderChecker::capture_error_flag && UVCPHeaderChecker::capture_image_flag) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 1.0f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.60f)); 
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.80f)); 
            }
            if (ImGui::Button(UVCPHeaderChecker::capture_error_flag ? "Error Image" : "Error Image", ImVec2(96, 40))) {
                if (UVCPHeaderChecker::capture_image_flag){
                    UVCPHeaderChecker::capture_error_flag = !UVCPHeaderChecker::capture_error_flag;
                }
            }
            ImGui::PopStyleColor(2);

            ImGui::SetCursorPos(ImVec2(247, 185));
            if (UVCPHeaderChecker::capture_suspicious_flag && UVCPHeaderChecker::capture_image_flag) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 1.0f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.60f)); 
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.80f)); 
            }
            if (ImGui::Button(UVCPHeaderChecker::capture_suspicious_flag ? "Suspicious Image" : "Suspicious Image", ImVec2(96, 40))) {
                if (UVCPHeaderChecker::capture_image_flag){
                    UVCPHeaderChecker::capture_suspicious_flag = !UVCPHeaderChecker::capture_suspicious_flag;
                }
            }
            ImGui::PopStyleColor(2);

            ImGui::SetCursorPos(ImVec2(357, 185));
            if (UVCPHeaderChecker::capture_valid_flag && UVCPHeaderChecker::capture_image_flag) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 1.0f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.60f)); 
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.80f)); 
            }
            if (ImGui::Button(UVCPHeaderChecker::capture_valid_flag ? "Valid Image" : "Valid Image", ImVec2(96, 40))) {
                if (UVCPHeaderChecker::capture_image_flag){
                    UVCPHeaderChecker::capture_valid_flag = !UVCPHeaderChecker::capture_valid_flag;
                }
            }
            ImGui::PopStyleColor(2);

            ImGui::SetCursorPos(ImVec2(27, 240));
            if (UVCPHeaderChecker::filter_on_off_flag) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 1.0f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.60f)); 
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.80f)); 
            }
            if (ImGui::Button(UVCPHeaderChecker::filter_on_off_flag ? "Filter ON" : "Filter OFF", ImVec2(96, 40))){
                UVCPHeaderChecker::filter_on_off_flag = !UVCPHeaderChecker::filter_on_off_flag;
                if (UVCPHeaderChecker::filter_on_off_flag) {
                    UVCPHeaderChecker::irregular_define_flag = prev_static_const_img_filter;
                    UVCPHeaderChecker::pts_decrease_filter_flag = prev_pts_decrease_filter;
                    UVCPHeaderChecker::stc_decrease_filter_flag = prev_scr_stc_decrease_filter;
                } else {
                    // if (static_const_img_filter || pts_decrease_filter || scr_stc_decrease_filter) {
                        prev_static_const_img_filter = UVCPHeaderChecker::irregular_define_flag;
                        prev_pts_decrease_filter = UVCPHeaderChecker::pts_decrease_filter_flag;
                        prev_scr_stc_decrease_filter = UVCPHeaderChecker::stc_decrease_filter_flag;
                    // }

                    UVCPHeaderChecker::irregular_define_flag = false;
                    UVCPHeaderChecker::pts_decrease_filter_flag = false;
                    UVCPHeaderChecker::stc_decrease_filter_flag = false;
                }
            }
            ImGui::PopStyleColor(2);

            ImGui::SetCursorPos(ImVec2(137, 240));
            if (UVCPHeaderChecker::irregular_define_flag && UVCPHeaderChecker::filter_on_off_flag) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 1.0f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.60f)); 
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.80f)); 
            }
            if (ImGui::Button(UVCPHeaderChecker::irregular_define_flag ? "Irregular" : "Irregular", ImVec2(96, 40))) {
                if (UVCPHeaderChecker::filter_on_off_flag){
                    UVCPHeaderChecker::irregular_define_flag = !UVCPHeaderChecker::irregular_define_flag;  
                }
            }
            ImGui::PopStyleColor(2);

            ImGui::SetCursorPos(ImVec2(247, 240));
            if (UVCPHeaderChecker::pts_decrease_filter_flag && UVCPHeaderChecker::filter_on_off_flag) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 1.0f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.60f)); 
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.80f)); 
            }
            if (ImGui::Button(UVCPHeaderChecker::pts_decrease_filter_flag ? "PTS Dec" : "PTS Dec", ImVec2(96, 40))) {
                if (UVCPHeaderChecker::filter_on_off_flag){
                    UVCPHeaderChecker::pts_decrease_filter_flag = !UVCPHeaderChecker::pts_decrease_filter_flag;  
                }
            }
            ImGui::PopStyleColor(2);

            ImGui::SetCursorPos(ImVec2(357, 240));
            if (UVCPHeaderChecker::stc_decrease_filter_flag && UVCPHeaderChecker::filter_on_off_flag) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 1.0f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.60f)); 
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.80f)); 
            }
            if (ImGui::Button(UVCPHeaderChecker::stc_decrease_filter_flag ? "STC Dec" : "STC Dec", ImVec2(96, 40))) {
                if(UVCPHeaderChecker::filter_on_off_flag){ 
                    UVCPHeaderChecker::stc_decrease_filter_flag = !UVCPHeaderChecker::stc_decrease_filter_flag;  
                }
            }
            ImGui::PopStyleColor(2);


            ImGui::SetCursorPos(ImVec2(247, 295));
            if (UVCPHeaderChecker::play_pause_flag) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 1.0f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.60f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.10f)); 
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.50f)); 
            }
            if (ImGui::Button(UVCPHeaderChecker::play_pause_flag ? "Play" : "Pause", ImVec2(96, 40))) {
                UVCPHeaderChecker::play_pause_flag = ! UVCPHeaderChecker::play_pause_flag;
            }
            ImGui::PopStyleColor(2);

            ImGui::SetCursorPos(ImVec2(357, 295));
            if (ImGui::Button("Quit", ImVec2(96, 40))) {
                exit(0);
            }

            ImGui::SetCursorPos(ImVec2(9, 310));
            lb_data.print_customtext();
            ImGui::End();
        } 

        // **Window 13 - Valid Frame **
        {
            WindowData& vf_data = uvcfd_win.getWin_ValidFrame();
            ImGui::SetNextWindowPos(vf_data.get_initial_position(), ImGuiCond_Always);
            ImGui::SetNextWindowSize(vf_data.get_window_size(), ImGuiCond_Always);

            ImGui::Begin("Valid Frame Data", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            vf_data.print_customtext();
            
            float current_scroll_y = ImGui::GetScrollY();
            float max_scroll_y = ImGui::GetScrollMaxY();
            if (current_scroll_y >= max_scroll_y) {
                ImGui::SetScrollHereY(1.0f);
            }

            ImGui::End();
        }

        // **Window 0 - Frame Data**
        {
            WindowData& ef_data = uvcfd_win.getWin_ErrorFrame();
            ImGui::SetNextWindowPos(ef_data.get_initial_position(), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ef_data.get_window_size(), ImGuiCond_Always);

            ImGui::Begin("Error Frame Data", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            if (show_error_log && selected_error_frame < ef_data.errorlogtext_size()){
                ef_data.print_errorlogtext(selected_error_frame);
                if (show_image) {
                    ImGui::Text("Image:");

                    if (texture_id) {
                        ImGui::Image((ImTextureID)(intptr_t)texture_id, ImVec2(288, 162));
                    } else {
                        ImGui::Text("Invalid Image / Failed to load image. Image could be zero size or not found.");
                    }
                }
            } else if (show_suspicious_log && selected_suspicious_frame < ef_data.suspiciouslogtext_size()) {
                ef_data.print_suspiciouslogtext(selected_suspicious_frame);
                if (show_image) {
                    ImGui::Text("Image:");

                    if (texture_id) {
                        ImGui::Image((ImTextureID)(intptr_t)texture_id, ImVec2(288, 162));
                    } else {
                        ImGui::Text("Invalid Image / Failed to load image. Image could be zero size or not found.");
                    }
                }
            } else {
                ef_data.print_customtext();
            }

            float current_scroll_y = ImGui::GetScrollY();
            float max_scroll_y = ImGui::GetScrollMaxY();
            if (current_scroll_y >= max_scroll_y) {
                ImGui::SetScrollHereY(1.0f);
            }

            ImGui::End();
        }

        // **Window 1 - Time Data**
        {
            WindowData& ft_data = uvcfd_win.getWin_FrameTime();
            ImGui::SetNextWindowPos(ft_data.get_initial_position(), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ft_data.get_window_size(), ImGuiCond_Always);

            ImGui::Begin("Error Frame: Time & Payload Size Data", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            if (show_error_log && selected_error_frame < ft_data.errorlogtext_size()) {
                ft_data.print_errorlogtext(selected_error_frame);
            } else if (show_suspicious_log && selected_suspicious_frame < ft_data.suspiciouslogtext_size()) {
                ft_data.print_suspiciouslogtext(selected_suspicious_frame);
            }else {
                ft_data.print_customtext();
            }
            
            float current_scroll_y = ImGui::GetScrollY();
            float max_scroll_y = ImGui::GetScrollMaxY();
            if (current_scroll_y >= max_scroll_y) {
                ImGui::SetScrollHereY(1.0f);
            }
            ImGui::End();
        }

        // **Window 2 - Summary**
        {
            WindowData& sm_data = uvcfd_win.getWin_Summary();
            ImGui::SetNextWindowPos(sm_data.get_initial_position(), ImGuiCond_Always);
            ImGui::SetNextWindowSize(sm_data.get_window_size(), ImGuiCond_Always);

            ImGui::Begin("Summary", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            if (show_error_log && selected_error_frame < sm_data.errorlogtext_size()) {
                sm_data.print_errorlogtext(selected_error_frame);
            } else if(show_suspicious_log && selected_suspicious_frame < sm_data.suspiciouslogtext_size()) {
                sm_data.print_suspiciouslogtext(selected_suspicious_frame);
            }else {
                sm_data.print_customtext();
            }

            ImGui::End();
        }

        // **Window 3 - Control**
        {
            WindowData& cc_data = uvcfd_win.getWin_ControlConfig();
            ImGui::SetNextWindowPos(cc_data.get_initial_position(), ImGuiCond_Always);
            ImGui::SetNextWindowSize(cc_data.get_window_size(), ImGuiCond_Always);

            ImGui::Begin("Control Config", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            ImGui::Text("Current Defined Configs:");
            cc_data.print_customtext();

            float current_scroll_y = ImGui::GetScrollY();
            float max_scroll_y = ImGui::GetScrollMaxY();
            if (current_scroll_y >= max_scroll_y) {
                ImGui::SetScrollHereY(1.0f);
            }

            ImGui::End();
        }

        // **Window 4 - Statistics**
        {
            WindowData& st_data = uvcfd_win.getWin_Statistics();
            ImGui::SetNextWindowPos(st_data.get_initial_position(), ImGuiCond_Always);
            ImGui::SetNextWindowSize(st_data.get_window_size(), ImGuiCond_Always);

            ImGui::Begin("Statistics", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            st_data.print_customtext();
            ImGui::End();
        }

        // **Window 5 - Debug**
        {
            WindowData& db_data = uvcfd_win.getWin_Debug();
            ImGui::SetNextWindowPos(db_data.get_initial_position(), ImGuiCond_Always);
            ImGui::SetNextWindowSize(db_data.get_window_size(), ImGuiCond_Always);

            ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            db_data.print_customtext();
            
            float current_scroll_y = ImGui::GetScrollY();
            float max_scroll_y = ImGui::GetScrollMaxY();
            if (current_scroll_y >= max_scroll_y) {
                ImGui::SetScrollHereY(1.0f);
            }

            ImGui::End();
        }

        // **Window 6 - prev **
        {
            WindowData& pv_data = uvcfd_win.getWin_PreviousValid();
            ImGui::SetNextWindowPos(pv_data.get_initial_position(), ImGuiCond_Always);
            ImGui::SetNextWindowSize(pv_data.get_window_size(), ImGuiCond_Always);

            ImGui::Begin("Previous Valid Data", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            if (show_error_log && selected_error_frame < pv_data.e3plogtext_size()) {
                if (selected_error_payload < pv_data.e3plogtext_frame_size(selected_error_frame)) {
                    pv_data.print_e3p_logtext(selected_error_frame, selected_error_payload);
                } else {
                    pv_data.print_e3p_logtext(selected_error_frame, 0);
                }
            } else if(show_suspicious_log) {
                    ImGui::Text("-");
            } else {
                pv_data.print_customtext();
            }
            ImGui::End();
        }

        // **Window 7 - inbetween error **
        {
            WindowData& ie_data = uvcfd_win.getWin_LostInbetweenError();
            ImGui::SetNextWindowPos(ie_data.get_initial_position(), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ie_data.get_window_size(), ImGuiCond_Always);

            ImGui::Begin("Lost Inbetween Error Data", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            if (show_error_log && selected_error_frame < ie_data.e3plogtext_size()) {
                if (selected_error_payload < ie_data.e3plogtext_frame_size(selected_error_frame)) {
                    ie_data.print_e3p_logtext(selected_error_frame, selected_error_payload);
                } else {
                    ie_data.print_e3p_logtext(selected_error_frame, 0);
                }
            } else if(show_suspicious_log) {
                    ImGui::Text("-");
            } else {
                ie_data.print_customtext();
            }
            ImGui::End();
        }

        // **Window 8 - current **
        {
            WindowData& ce_data = uvcfd_win.getWin_CurrentError();
            ImGui::SetNextWindowPos(ce_data.get_initial_position(), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ce_data.get_window_size(), ImGuiCond_Always);

            ImGui::Begin("Current Error Data", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoResize);
            if (show_error_log && selected_error_frame < ce_data.e3plogtext_size()) {
                if (selected_error_payload < ce_data.e3plogtext_frame_size(selected_error_frame)) {
                    ce_data.print_e3p_logtext(selected_error_frame, selected_error_payload);
                } else {
                    ce_data.print_e3p_logtext(selected_error_frame, 0);
                }
            } else if(show_suspicious_log) {
                    ImGui::Text("-");
            } else {
                ce_data.print_customtext();
            }

            ImGui::End();
        }
        
        // **Graph 0 - Histogram **
        {
            WindowData& gw_data = uvcfd_win.getWin_GraphWindow();
            GraphData& URB_graph_data = uvcfd_graph.getGraph_URBGraph();
            GraphData& SOF_graph_data = uvcfd_graph.getGraph_SOFGraph();
            GraphData& PTS_graph_data = uvcfd_graph.getGraph_PTSGraph();            

            ImGui::SetNextWindowPos(gw_data.get_initial_position(), ImGuiCond_Always);
            ImGui::SetNextWindowSize(gw_data.get_window_size(), ImGuiCond_Always);

            ImGui::Begin("Histogram", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

            URB_graph_data.update_max_graph_height_of_all_time();
            SOF_graph_data.update_max_graph_height_of_all_time();
            PTS_graph_data.update_max_graph_height_of_all_time();

            if (show_error_log && selected_error_frame < URB_graph_data.get_error_log_graph_data_size()) {
                URB_graph_data.show_error_log_info(selected_error_frame);
                // PTS_graph_data.show_error_log_info(selected_error_frame);
                URB_graph_data.show_error_graph_data(selected_error_frame);
                SOF_graph_data.show_error_graph_data(selected_error_frame);
                PTS_graph_data.show_error_graph_data(selected_error_frame);
            } else if(show_suspicious_log && selected_suspicious_frame < URB_graph_data.get_suspicious_log_graph_data_size()) {
                URB_graph_data.show_suspicious_log_info(selected_suspicious_frame);
                // PTS_graph_data.show_suspicious_log_info(selected_suspicious_frame);
                URB_graph_data.show_suspicious_graph_data(selected_suspicious_frame);
                SOF_graph_data.show_suspicious_graph_data(selected_suspicious_frame);
                PTS_graph_data.show_suspicious_graph_data(selected_suspicious_frame);
            } else {
                URB_graph_data.show_stream_info();
                // PTS_graph_data.show_stream_info();
                URB_graph_data.show_current_graph_data();
                SOF_graph_data.show_current_graph_data();
                PTS_graph_data.show_current_graph_data();
            }

            ImGui::End();
        }

        ImGui::Render();
        glViewport(0, 0, 1920, 1080);
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(41));
    }
}

void end_screen(){
    std::cout << "end_screen()" << std::endl;
    WindowManager& uvcfd_win = WindowManager::getInstance();

    finish_imgui();

    if (window) { 
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

}
