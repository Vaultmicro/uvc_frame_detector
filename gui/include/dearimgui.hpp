#ifndef DEARIMGUI_HPP
#define DEARIMGUI_HPP

#include <GL/glew.h>              
#include <GLFW/glfw3.h>

extern GLFWwindow* window;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool init_imgui();
void finish_imgui();

#endif  // DEARIMGUI_HPP
