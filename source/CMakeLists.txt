
set(DEVELOPE_PHOTO_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/image_develope/develope_photo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/image_develope/rgb_to_jpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/image_develope/yuyv_to_rgb.cpp
)

add_executable(
    uvcfd
    ${CMAKE_CURRENT_SOURCE_DIR}/moncapwer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/validuvc/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/validuvc/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/validuvc/device_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/gui_win.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/window_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/dearimgui.cpp
    ${DEVELOPE_PHOTO_SOURCES}
    ${IMGUI_SOURCES}
)

target_compile_definitions(uvcfd PUBLIC GUI_SET GLEW_STATIC)

if (WIN32)
    target_link_libraries(uvcfd
        PRIVATE
        ${GLEW_LIBRARIES_STATIC}
        glfw
        OpenGL::GL
        ${LIBJPEG_TURBO_LIBRARIES}
    )
else()
    target_link_libraries(uvcfd
        PRIVATE
        ${GLEW_LIBRARIES_STATIC}
        glfw
        OpenGL::GL
        ${LIBJPEG_TURBO_LIBRARIES}
        ${X11_LIBRARIES}
    )
endif()

add_executable(
    oldmanandsea
    ${CMAKE_CURRENT_SOURCE_DIR}/moncapwer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/validuvc/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/validuvc/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/validuvc/device_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/verbose.cpp
    ${DEVELOPE_PHOTO_SOURCES}
)

target_compile_definitions(oldmanandsea PUBLIC CLI_SET)

target_link_libraries(oldmanandsea
      ${LIBJPEG_TURBO_LIBRARIES}
)

# add_subdirectory(validuvc/linux)