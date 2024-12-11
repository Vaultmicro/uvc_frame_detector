# cmake_minimum_required(VERSION 3.15)

# project(
#     uvcfd_test
#     DESCRIPTION "Test Code for uvcfd"
#     VERSION 0.3.5
#     LANGUAGES CXX C
# )

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_C_STANDARD 99)
# set(CMAKE_C_STANDARD_REQUIRED True)

# if (WIN32)
#     set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
#     add_compile_definitions(
#         $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=2>
#         $<$<CONFIG:Release>:_ITERATOR_DEBUG_LEVEL=0>
#     )
# endif()

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Include Directories
include_directories(
    ${googletest_SOURCE_DIR}/googlemock/include
    ${googletest_SOURCE_DIR}/googletest/include
    ${CMAKE_SOURCE_DIR}/include
)

# Common Sources
set(COMMON_SOURCES
    ${CMAKE_SOURCE_DIR}/source/validuvc/uvcpheader_checker.cpp
    ${CMAKE_SOURCE_DIR}/source/validuvc/control_config.cpp
    ${CMAKE_SOURCE_DIR}/source/utils/verbose.cpp
    ${CMAKE_SOURCE_DIR}/source/image_develope/develope_photo.cpp
    ${CMAKE_SOURCE_DIR}/source/image_develope/rgb_to_jpeg.cpp
    ${CMAKE_SOURCE_DIR}/source/image_develope/yuyv_to_rgb.cpp
)

# Add Test Executables
function(add_uvc_test target_name source_file)
    add_executable(${target_name} ${source_file} ${COMMON_SOURCES})
    target_link_libraries(${target_name} PRIVATE gtest gtest_main ${LIBJPEG_TURBO_LIBRARIES})
endfunction()

# Test Targets
add_uvc_test(valid_test ${CMAKE_SOURCE_DIR}/tests/valid_test.cpp)
add_uvc_test(frame_test_bulk ${CMAKE_SOURCE_DIR}/tests/frame_test_bulk.cpp)
add_uvc_test(frame_test_iso ${CMAKE_SOURCE_DIR}/tests/frame_test_iso.cpp)

# Packet Handler Test (UNIX only)
if (UNIX)
    add_executable(
        test_packet_handler
        ${CMAKE_SOURCE_DIR}/tests/test_packet_handler.cpp
        ${CMAKE_SOURCE_DIR}/source/moncapler.cpp
        ${COMMON_SOURCES}
    )
    target_compile_definitions(test_packet_handler PRIVATE UNIT_TEST)
    target_link_libraries(test_packet_handler PRIVATE gtest gtest_main ${PCAP_LIBRARIES} ${LIBJPEG_TURBO_LIBRARIES})

    add_executable(
        show_urb_header
        ${CMAKE_SOURCE_DIR}/tests/show_urb_header.cpp
        ${CMAKE_SOURCE_DIR}/source/moncapler.cpp
        ${COMMON_SOURCES}
    )
    target_compile_definitions(show_urb_header PRIVATE UNIT_TEST)
    target_link_libraries(show_urb_header PRIVATE ${PCAP_LIBRARIES} ${LIBJPEG_TURBO_LIBRARIES})
endif()

# Log Tests
add_executable(log_test ${CMAKE_SOURCE_DIR}/tests/log_test.cpp ${COMMON_SOURCES})
target_link_libraries(log_test PRIVATE ${LIBJPEG_TURBO_LIBRARIES})

add_executable(
    log_test_g
    ${CMAKE_SOURCE_DIR}/tests/log_test.cpp
    ${COMMON_SOURCES}
    ${CMAKE_SOURCE_DIR}/source/gui/gui_win.cpp
    ${CMAKE_SOURCE_DIR}/source/gui/window_manager.cpp
    ${CMAKE_SOURCE_DIR}/source/gui/dearimgui.cpp
    ${IMGUI_SOURCES}
)

target_compile_definitions(log_test_g PUBLIC GUI_SET GLEW_STATIC)

if (WIN32)
target_link_libraries(log_test_g
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

# Example
add_executable(
    example 
    ${CMAKE_SOURCE_DIR}/examples/smpte.cpp 
    ${COMMON_SOURCES}
)

target_link_libraries(example PRIVATE ${LIBJPEG_TURBO_LIBRARIES} )


