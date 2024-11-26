// gui_win.hpp

#ifndef GUI_WIN_HPP
#define GUI_WIN_HPP

#include <iostream>
#include <chrono>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "window_manager.hpp"
#include "dearimgui.hpp"
#include <atomic>

// Global variable declaration
extern int gui_window_number;
extern int print_whole_flag;
extern int temp_window_number;
extern int frame_error_flag;
extern int frame_suspicious_flag;

void addErrorFrameLog(const std::string& efn);
const std::vector<std::string>& getErrorFrameLog();

void addSuspiciousFrameLog(const std::string& efn);
const std::vector<std::string>& getSuspiciousFrameLog();


GLuint LoadTextureFromFile(const char* filename);


// Function declarations
int start_screen();
void screen();
void end_screen();

#endif // GUI_WIN_HPP
