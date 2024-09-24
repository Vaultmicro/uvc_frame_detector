cmake_minimum_required(VERSION 3.10)

project(
    uvc_frame_detector
    DESCRIPTION "Packet Capture And Analysis Tool for UVC Frame"
    VERSION 0.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)


#find_package(PkgConfig REQUIRED)
#pkg_check_modules(PCAP REQUIRED libpcap)
set(PCAP_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/library/libpcap.so
    ${CMAKE_CURRENT_SOURCE_DIR}/library/libpcap.so.1.10.4
)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${googletest_SOURCE_DIR}/googlemock/include)
include_directories(${googletest_SOURCE_DIR}/googletest/include)


add_executable(
    moncapler
    ${CMAKE_CURRENT_SOURCE_DIR}/source/moncapler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uvcpheader_checker.cpp
)


target_link_libraries(
    moncapler
    PRIVATE
        ${PCAP_LIBRARIES}
)
