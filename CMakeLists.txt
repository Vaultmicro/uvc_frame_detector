cmake_minimum_required(VERSION 3.15)

project(
    uvc_frame_detector_aka_oldmanandsea
    DESCRIPTION "Packet Capture And Analysis Tool for UVC Frame [UVC_IFD] stands for USB Video Class Invalid Frame Detector"
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



if (WIN32)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
set(GUI_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/lib/glfw3.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/lib/glew32s.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/lib/OpenGL32.lib
)
set(LIBJPEGTURBO_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/library/libjpeg-turbo64/lib/turbojpeg-static.lib
)

elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
set(GUI_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/lib/glfw3.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/lib/glew32s.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/lib/OpenGL32.lib
)

set(LIBJPEGTURBO_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/library/libjpeg-turbo/lib/turbojpeg-static.lib
)

endif()
elseif (UNIX)
set(LIBUSB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include/libusb-1.0)
set(LIBUSB_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/library/libusb-1.0.so.0.4.0)
set(PCAP_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/library/libpcap.so
    ${CMAKE_CURRENT_SOURCE_DIR}/library/libpcap.so.1.10.4
)
set(GUI_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/lib/libglfw.so
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/lib/libGLEW.a
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/lib/libGL.so
)
set(LIBJPEGTURBO_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/library/libjpeg-turbo64/lib/libturbojpeg.a
)

endif()

include_directories(${googletest_SOURCE_DIR}/googlemock/include)
include_directories(${googletest_SOURCE_DIR}/googletest/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gui/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/library/libjpeg-turbo64/include)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/library/libjpeg-turbo/include)
endif()

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
    ${CMAKE_CURRENT_SOURCE_DIR}/source/image_develope/develope_photo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/image_develope/rgb_to_jpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/image_develope/yuyv_to_rgb.cpp
)

  
if(UNIX)
add_executable(
    uvc_frame_detector
    ${CMAKE_CURRENT_SOURCE_DIR}/source/validuvc/moncapler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/validuvc/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/validuvc/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/utils/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/utils/logger.cpp
    ${DEVELOPE_PHOTO_SOURCES}
)

target_link_libraries(
    uvc_frame_detector
    PRIVATE
        ${PCAP_LIBRARIES}
        ${LIBJPEGTURBO_LIBRARIES}
)
endif(UNIX)

add_executable(
    oldmanandsea
    ${CMAKE_CURRENT_SOURCE_DIR}/source/validuvc/moncapwer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/validuvc/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/validuvc/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/utils/verbose.cpp
    ${DEVELOPE_PHOTO_SOURCES}
)

target_link_libraries(oldmanandsea
    ${LIBJPEGTURBO_LIBRARIES}
)

if(WIN32)
set(CMAKE_BUILD_TYPE Release)

add_executable(
    oldmanandsea_t
    ${CMAKE_CURRENT_SOURCE_DIR}/source/validuvc/moncapwer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/validuvc/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/validuvc/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/utils/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/tui/tui_win.cpp
    ${DEVELOPE_PHOTO_SOURCES}
)

target_link_libraries(
    oldmanandsea_t
    PRIVATE
        ${LIBJPEGTURBO_LIBRARIES}
)
target_compile_definitions(oldmanandsea_t PUBLIC TUI_SET)
endif(WIN32)


add_executable(
    uvcfd
    ${CMAKE_CURRENT_SOURCE_DIR}/source/validuvc/moncapwer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/validuvc/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/validuvc/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/utils/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gui/gui_win.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gui/window_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gui/dearimgui.cpp
    ${DEVELOPE_PHOTO_SOURCES}
    ${IMGUI_SOURCES}
)

target_compile_definitions(uvcfd PUBLIC GUI_SET GLEW_STATIC)

target_link_libraries(uvcfd
    ${GUI_LIBRARIES}
    ${LIBJPEGTURBO_LIBRARIES}
)



if (UNIX)
add_executable(
    uvcperf
    ${CMAKE_CURRENT_SOURCE_DIR}/source/validuvc/uvc_stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/libuvc/stream.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/libuvc/ctrl-gen.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/libuvc/device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/libuvc/diag.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/libuvc/init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/validuvc/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/validuvc/control_config.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/source/utils/verbose.cpp
    ${DEVELOPE_PHOTO_SOURCES}
)

target_link_libraries(
    uvcperf
    PRIVATE
        ${LIBUSB_LIBRARIES}
        ${LIBJPEGTURBO_LIBRARIES}

)
endif(UNIX)


# add_subdirectory(tests)