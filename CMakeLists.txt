cmake_minimum_required(VERSION 3.10)

project(
    uvc_frame_detector
    DESCRIPTION "Packet Capture And Analysis Tool for UVC Frame"
    VERSION 0.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
# )
# FetchContent_MakeAvailable(googletest)


#find_package(PkgConfig REQUIRED)
#pkg_check_modules(PCAP REQUIRED libpcap)
set(PCAP_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/library/libpcap.so
    ${CMAKE_CURRENT_SOURCE_DIR}/library/libpcap.so.1.10.4
)

set(NPCAP_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/library/npcap_sdk_lib_1.13/x64/Packet.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/library/npcap_sdk_lib_1.13/x64/wpcap.lib
)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${googletest_SOURCE_DIR}/googlemock/include)
include_directories(${googletest_SOURCE_DIR}/googletest/include)


add_executable(
    uvc_frame_detector
    ${CMAKE_CURRENT_SOURCE_DIR}/source/moncapler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp
)

if (UNIX)
    target_link_libraries(
        uvc_frame_detector
        PRIVATE
            ${PCAP_LIBRARIES}
    )
endif(UNIX)
if (WIN32)
    target_link_libraries(
        uvc_frame_detector
        PRIVATE
            ${NPCAP_LIBRARIES}
    )
endif (WIN32)



# add_executable(
#     example
#     ${CMAKE_CURRENT_SOURCE_DIR}/examples/smpte.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp
# )

# add_executable(
#     valid_test
#     ${CMAKE_CURRENT_SOURCE_DIR}/tests/valid_test.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp
# )

# target_link_libraries(
#     valid_test
#     PRIVATE
#         gtest
#         gtest_main
# )

# add_executable(
#     frame_test_bulk
#     ${CMAKE_CURRENT_SOURCE_DIR}/tests/frame_test_bulk.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp 
# )

# target_link_libraries(
#     frame_test_bulk
#     PRIVATE
#         gtest
#         gtest_main 
# )

# add_executable(
#     frame_test_iso
#     ${CMAKE_CURRENT_SOURCE_DIR}/tests/frame_test_iso.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp 
# )

# target_link_libraries(
#     frame_test_iso
#     PRIVATE
#         gtest
#         gtest_main
# )


# add_executable(
#     test_packet_handler
#     ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_packet_handler.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/moncapler.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp
# )

# target_compile_definitions(test_packet_handler PRIVATE UNIT_TEST)

# target_link_libraries(
#     test_packet_handler
#     PRIVATE
#         gtest
#         gtest_main
#         ${PCAP_LIBRARIES}
# )



# add_executable(
#     show_urb_header
#     ${CMAKE_CURRENT_SOURCE_DIR}/tests/show_urb_header.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/moncapler.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/control_config.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/verbose.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp
# )

# target_compile_definitions(show_urb_header PRIVATE UNIT_TEST)

# target_link_libraries(
#     show_urb_header
#     PRIVATE
#         ${PCAP_LIBRARIES}
# )
