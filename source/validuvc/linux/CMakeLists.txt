

if (UNIX AND NOT APPLE)

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

endif(UNIX AND NOT APPLE)