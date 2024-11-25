#include "gui/dearimgui.hpp"
#include <GL/glew.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <iostream>
#include <stb_image/stb_image.h>
#include <GLFW/glfw3.h>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <windows.h>
#include <GLFW/glfw3native.h>
#endif


#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif

#ifdef __APPLE__
#include <objc/objc.h>
#include <objc/runtime.h>
#endif

void remove_close_button(GLFWwindow* window) {
#ifdef _WIN32
    HWND hwnd = glfwGetWin32Window(window);
    if (hwnd) {
        HMENU hMenu = GetSystemMenu(hwnd, FALSE);
        if (hMenu) {
            DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
        }
    }
#endif

#ifdef __linux__
    Display* display = glfwGetX11Display();
    Window x11Window = glfwGetX11Window(window);

    Atom wmHints = XInternAtom(display, "_MOTIF_WM_HINTS", True);
    if (wmHints) {
        struct {
            unsigned long flags;
            unsigned long functions;
            unsigned long decorations;
            long input_mode;
            unsigned long status;
        } hints = {2, ~0UL & ~1UL, 0, 0, 0}; 

        XChangeProperty(display, x11Window, wmHints, wmHints, 32, PropModeReplace,
                        (unsigned char*)&hints, sizeof(hints) / sizeof(long));
        XFlush(display);
    }
#endif

#ifdef __APPLE__
    id cocoaWindow = glfwGetCocoaWindow(window);
    id closeButton = [cocoaWindow standardWindowButton:NSWindowCloseButton];
    [closeButton setHidden:YES]; 
#endif
}

void set_window_icon(GLFWwindow* window, const char* iconPath) {
    int width, height, channels;
    unsigned char* data = stbi_load(iconPath, &width, &height, &channels, 4);

    if (!data) {
        std::cerr << "Failed to load icon image: " << iconPath << std::endl;
        return;
    }

    GLFWimage icon;
    icon.width = width;
    icon.height = height;
    icon.pixels = data;

    glfwSetWindowIcon(window, 1, &icon);

    stbi_image_free(data);
}



GLFWwindow* window = nullptr;  

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Dear ImGui, OpenGL, GLFW initialization
bool init_imgui() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1920, 1050, "Vaultmicro USB Video Class Frame Detector", NULL, NULL);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    remove_close_button(window);
    set_window_icon(window, "icon.png");

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return true;
}

void finish_imgui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}
