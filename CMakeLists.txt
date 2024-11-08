cmake_minimum_required(VERSION 3.15)

project(
    uvc_frame_detector
    DESCRIPTION "Packet Capture And Analysis Tool for UVC Frame"
    VERSION 0.3.3
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

if (WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    add_compile_definitions(
        $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=2>
        $<$<CONFIG:Release>:_ITERATOR_DEBUG_LEVEL=0>
    )
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

if(UNIX)
set(LIBUSB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include/libusb-1.0)
set(LIBUSB_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/library/libusb-1.0.so.0.4.0)
set(PCAP_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/library/libpcap.so
    ${CMAKE_CURRENT_SOURCE_DIR}/library/libpcap.so.1.10.4
)
endif(UNIX)

set(GUI_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/lib/glfw3.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/lib/glew32s.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/lib/OpenGL32.lib
)

set(LIBJPEGTURBO64_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/library/libjpeg-turbo64/lib/turbojpeg.lib
)

include_directories(${googletest_SOURCE_DIR}/googlemock/include)
include_directories(${googletest_SOURCE_DIR}/googletest/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gui/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gui/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/library/libjpeg-turbo64/include)

set(IMGUI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui/imgui_impl_opengl3.cpp
)

set(DEVELOPE_PHOTO_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/develope_photo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/rgb_to_jpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/yuyv_to_rgb.cpp
)

  
if(UNIX)
add_executable(
    uvc_frame_detector
    ${CMAKE_CURRENT_SOURCE_DIR}/source/moncapler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${DEVELOPE_PHOTO_SOURCES}
)

target_link_libraries(
    uvc_frame_detector
    PRIVATE
        ${PCAP_LIBRARIES}
        ${LIBJPEGTURBO64_LIBRARIES}
)
endif(UNIX)

add_executable(
    oldmanandsea
    ${CMAKE_CURRENT_SOURCE_DIR}/source/moncapwer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${DEVELOPE_PHOTO_SOURCES}
)

target_link_libraries(oldmanandsea
    ${LIBJPEGTURBO64_LIBRARIES}
)

if(WIN32)
set(CMAKE_BUILD_TYPE Release)

add_executable(
    oldmanandsea_t
    ${CMAKE_CURRENT_SOURCE_DIR}/source/moncapwer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/tui_win.cpp
    ${DEVELOPE_PHOTO_SOURCES}
)

target_link_libraries(
    oldmanandsea_t
    PRIVATE
        ${LIBJPEGTURBO64_LIBRARIES}
)
target_compile_definitions(oldmanandsea_t PUBLIC TUI_SET)
endif(WIN32)


add_executable(
    oldmanandsea_g
    ${CMAKE_CURRENT_SOURCE_DIR}/source/moncapwer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/src/gui_win.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/src/dearimgui.cpp
    ${DEVELOPE_PHOTO_SOURCES}
    ${IMGUI_SOURCES}
)

target_compile_definitions(oldmanandsea_g PUBLIC GUI_SET GLEW_STATIC)

if (WIN32)
target_link_libraries(oldmanandsea_g
    ${GUI_LIBRARIES}
    ${LIBJPEGTURBO64_LIBRARIES}
)

elseif (UNIX)

target_link_libraries(oldmanandsea_g
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/lib/libglfw.so
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/lib/libGLEW.a
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/lib/libGL.so
        ${LIBJPEGTURBO64_LIBRARIES}
)
endif()

add_executable(
    example
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/smpte.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${DEVELOPE_PHOTO_SOURCES}
)
target_link_libraries(
    example
    PRIVATE
        ${LIBJPEGTURBO64_LIBRARIES}
)

if (UNIX)

add_executable(
    uvcperf
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvc_stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/libuvc/stream.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/libuvc/ctrl-gen.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/libuvc/device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/libuvc/diag.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/libuvc/init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${DEVELOPE_PHOTO_SOURCES}
)

target_link_libraries(
    uvcperf
    PRIVATE
        ${LIBUSB_LIBRARIES}
        ${LIBJPEGTURBO64_LIBRARIES}

)

endif(UNIX)

add_executable(
    valid_test
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/valid_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${DEVELOPE_PHOTO_SOURCES}
)

target_link_libraries(
    valid_test
    PRIVATE
        gtest
        gtest_main
        ${LIBJPEGTURBO64_LIBRARIES}

)


add_executable(
    frame_test_bulk
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/frame_test_bulk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${DEVELOPE_PHOTO_SOURCES}
)


target_link_libraries(
    frame_test_bulk
    PRIVATE
        gtest
        gtest_main
        ${LIBJPEGTURBO64_LIBRARIES}
)

add_executable(
    frame_test_iso
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/frame_test_iso.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${DEVELOPE_PHOTO_SOURCES}
)

target_link_libraries(
    frame_test_iso
    PRIVATE
        gtest
        gtest_main
        ${LIBJPEGTURBO64_LIBRARIES}
)


if (UNIX)

add_executable(
    test_packet_handler
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_packet_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/moncapler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${DEVELOPE_PHOTO_SOURCES}
)

target_compile_definitions(test_packet_handler PRIVATE UNIT_TEST)

target_link_libraries(
    test_packet_handler
    PRIVATE
        gtest
        gtest_main
        ${PCAP_LIBRARIES}
        ${LIBJPEGTURBO64_LIBRARIES}
)


add_executable(
    show_urb_header
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/show_urb_header.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/moncapler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${DEVELOPE_PHOTO_SOURCES}
)

target_compile_definitions(show_urb_header PRIVATE UNIT_TEST)

target_link_libraries(
    show_urb_header
    PRIVATE
        ${PCAP_LIBRARIES}
        ${LIBJPEGTURBO64_LIBRARIES}
)

endif(UNIX)


add_executable(
    log_test
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/log_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${DEVELOPE_PHOTO_SOURCES}
)
target_link_libraries(log_test
    ${LIBJPEGTURBO64_LIBRARIES}
)

if(WIN32)

add_executable(
    log_test_t
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/log_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/tui_win.cpp
    ${DEVELOPE_PHOTO_SOURCES}
)
target_compile_definitions(log_test_t PUBLIC TUI_SET)
target_link_libraries(log_test_t
    ${LIBJPEGTURBO64_LIBRARIES}
)

add_executable(
    log_test_g
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/log_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/src/gui_win.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/src/dearimgui.cpp
    ${IMGUI_SOURCES}
    ${DEVELOPE_PHOTO_SOURCES}
)

target_compile_definitions(log_test_g PUBLIC GUI_SET GLEW_STATIC)

target_link_libraries(
    log_test_g
    PRIVATE    
        ${GUI_LIBRARIES}
        ${LIBJPEGTURBO64_LIBRARIES}
)
endif(WIN32)
