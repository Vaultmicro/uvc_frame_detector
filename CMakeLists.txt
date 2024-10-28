cmake_minimum_required(VERSION 3.10)

project(
    uvc_frame_detector
    DESCRIPTION "Packet Capture And Analysis Tool for UVC Frame"
    VERSION 0.0.0
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

if (WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

endif(WIN32)

# find_package(PkgConfig REQUIRED)
# pkg_check_modules(PCAP REQUIRED libpcap)
if(UNIX)

set(LIBUSB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include/libusb-1.0)
set(LIBUSB_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/library/libusb-1.0.so.0.4.0)


set(PCAP_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/library/libpcap.so
    ${CMAKE_CURRENT_SOURCE_DIR}/library/libpcap.so.1.10.4
)
endif(UNIX)

include_directories(${googletest_SOURCE_DIR}/googlemock/include)
include_directories(${googletest_SOURCE_DIR}/googletest/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

if(WIN32)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gui/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gui/src)

link_directories(${CMAKE_SOURCE_DIR}/gui/lib)

set(IMGUI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui/imgui_impl_opengl3.cpp
)

endif(WIN32)



if(UNIX)
add_executable(
    uvc_frame_detector
    ${CMAKE_CURRENT_SOURCE_DIR}/source/moncapler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp
)

target_link_libraries(
        uvc_frame_detector
        PRIVATE
            ${PCAP_LIBRARIES}
    )
endif(UNIX)

add_executable(
    oldmanandsea
    ${CMAKE_CURRENT_SOURCE_DIR}/source/moncapwer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp
)

if(WIN32)
set(CMAKE_BUILD_TYPE Debug)

add_executable(
    oldmanandsea_t
    ${CMAKE_CURRENT_SOURCE_DIR}/source/moncapwer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/tui_win.cpp
)

target_compile_definitions(oldmanandsea_t PUBLIC TUI_SET)


add_executable(
    oldmanandsea_g
    ${CMAKE_CURRENT_SOURCE_DIR}/source/moncapwer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/src/gui_win.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/src/dearimgui.cpp
    ${IMGUI_SOURCES}
)

target_compile_definitions(oldmanandsea_g PUBLIC GUI_SET)


find_package(OpenGL REQUIRED)

target_link_libraries(oldmanandsea_g
    glfw3        # GLFW 
    glew32       # GLEW 
    ${OPENGL_LIBRARIES} # OpenGL 
)

add_custom_command(TARGET oldmanandsea_g POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/gui/lib/glew32.dll" $<TARGET_FILE_DIR:oldmanandsea_g>
)



endif(WIN32)


add_executable(
    example
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/smpte.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp
)

target_link_libraries(
    uvcperf
    PRIVATE
        ${LIBUSB_LIBRARIES}
)

endif(UNIX)

add_executable(
    valid_test
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/valid_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp
)

target_link_libraries(
    valid_test
    PRIVATE
        gtest
        gtest_main
)




add_executable(
    frame_test_bulk
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/frame_test_bulk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp 
)


target_link_libraries(
    frame_test_bulk
    PRIVATE
        gtest
        gtest_main 
)

add_executable(
    frame_test_iso
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/frame_test_iso.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp 
)

target_link_libraries(
    frame_test_iso
    PRIVATE
        gtest
        gtest_main
)


if (UNIX)

add_executable(
    test_packet_handler
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_packet_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/moncapler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp
)

target_compile_definitions(test_packet_handler PRIVATE UNIT_TEST)

target_link_libraries(
    test_packet_handler
    PRIVATE
        gtest
        gtest_main
        ${PCAP_LIBRARIES}
)


add_executable(
    show_urb_header
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/show_urb_header.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/moncapler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp
)

target_compile_definitions(show_urb_header PRIVATE UNIT_TEST)

target_link_libraries(
    show_urb_header
    PRIVATE
        ${PCAP_LIBRARIES}
)

endif(UNIX)


add_executable(
    log_test
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/log_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp 
)

if(WIN32)

add_executable(
    log_test_t
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/log_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/tui_win.cpp
)
target_compile_definitions(log_test_t PUBLIC TUI_SET)

endif(WIN32)
