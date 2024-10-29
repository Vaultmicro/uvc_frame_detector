// gui_win.hpp

#ifndef GUI_WIN_HPP
#define GUI_WIN_HPP

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

// Global variable declaration
extern int gui_window_number;
extern int print_whole_flag;

// Function declarations
int start_screen();
void screen();
void end_screen();

#endif // GUI_WIN_HPP
