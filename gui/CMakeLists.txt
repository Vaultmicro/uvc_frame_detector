cmake_minimum_required(VERSION 3.10)

project(MyImGuiProject)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    include
    imgui
    src
)

link_directories(${CMAKE_SOURCE_DIR}/lib)

set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
)

add_executable(MyImGuiProject 
src/screen.cpp
src/dearimgui.cpp
src/other_file.cpp
${IMGUI_SOURCES})

find_package(OpenGL REQUIRED)

target_link_libraries(MyImGuiProject
    glfw3        # GLFW 
    glew32       # GLEW 
    ${OPENGL_LIBRARIES} # OpenGL 
)

add_custom_command(TARGET MyImGuiProject POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/lib/glew32.dll" $<TARGET_FILE_DIR:MyImGuiProject>
)

