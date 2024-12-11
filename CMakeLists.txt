cmake_minimum_required(VERSION 3.15)

project(
    uvcfd
    DESCRIPTION "Packet Capture And Analysis Tool for UVC Frame [UVCFD] stands for USB Video Class Frame Detector"
    VERSION 0.3.5
    LANGUAGES CXX C
)

add_compile_options(/utf-8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

if (WIN32)
    add_compile_options(/utf-8)

    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    add_compile_definitions(
        $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=2>
        $<$<CONFIG:Release>:_ITERATOR_DEBUG_LEVEL=0>
    )
endif()

include(FetchContent)

if (UNIX)
# Fetch libusb
FetchContent_Declare(
  libusb
  GIT_REPOSITORY https://github.com/libusb/libusb
  GIT_TAG v1.0.27
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(libusb)

# Fetch libpcap
FetchContent_Declare(
  libpcap
  GIT_REPOSITORY https://github.com/the-tcpdump-group/libpcap
  GIT_TAG libpcap-1.10.5
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(libpcap)
endif(UNIX)

# Fetch ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG v1.91.5
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(imgui)

include_directories(${imgui_SOURCE_DIR})
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

if(WIN32)
  # Fetch GLEW
  FetchContent_Declare(
    glew_prebuilt
    URL https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0-win32.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  FetchContent_MakeAvailable(glew_prebuilt)

  include_directories(${glew_prebuilt_SOURCE_DIR}/include)

  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(GLEW_LIBRARIES_STATIC ${glew_prebuilt_SOURCE_DIR}/lib/Release/x64/glew32s.lib)
  elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(GLEW_LIBRARIES_STATIC ${glew_prebuilt_SOURCE_DIR}/lib/Release/Win32/glew32s.lib)
  endif()

else()
  # # Fetch GLEW
  # FetchContent_Declare(
  #   glew
  #     GIT_REPOSITORY https://github.com/nigels-com/glew.git
  #     GIT_TAG glew-2.2.0
  #     DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  # )
  # FetchContent_MakeAvailable(glew)
  find_package(X11 REQUIRED)
  include_directories(${X11_INCLUDE_DIR})

  set(GLEW_LIBRARIES_STATIC ${CMAKE_SOURCE_DIR}/library/libGLEW.a)

  include_directories(${glew_SOURCE_DIR}/include)

endif()

# Fetch GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw
  GIT_TAG 3.4
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(glfw)

include_directories(${glfw_SOURCE_DIR}/include)

set(WIN_LIB_FOUND TRUE)

if (WIN32)

  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/library/libjpeg-turbo64/lib/turbojpeg-static.lib")
      set(LIBJPEG_TURBO_LIBRARIES
          ${CMAKE_CURRENT_SOURCE_DIR}/library/libjpeg-turbo64/lib/turbojpeg-static.lib
      )
      include_directories(${CMAKE_CURRENT_SOURCE_DIR}/library/libjpeg-turbo64/include)
    else()
      message(WARNING "64-bit libjpeg-turbo library not found. Falling back to external project.")
      set(WIN_LIB_FOUND FALSE)
    endif()

  elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)

    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/library/libjpeg-turbo/lib/turbojpeg-static.lib")
      set(LIBJPEG_TURBO_LIBRARIES
          ${CMAKE_CURRENT_SOURCE_DIR}/library/libjpeg-turbo/lib/turbojpeg-static.lib
      )
      include_directories(${CMAKE_CURRENT_SOURCE_DIR}/library/libjpeg-turbo/include)

    else()
      message(WARNING "32-bit libjpeg-turbo library not found. Falling back to external project.")
      set(WIN_LIB_FOUND FALSE)
    endif()

  else()
    set(WIN_LIB_FOUND FALSE)
  endif()

  # elseif (UNIX)
  # set(LIBJPEG_TURBO_LIBRARIES
  #     ${CMAKE_CURRENT_SOURCE_DIR}/library/libjpeg-turbo64/lib/libturbojpeg.a
  # )

else()
  set(WIN_LIB_FOUND FALSE)
endif()

if (NOT WIN_LIB_FOUND)

  include(ExternalProject)
  # Fetch libjpeg-turbo
  ExternalProject_Add(
    libjpeg-turbo
    GIT_REPOSITORY https://github.com/libjpeg-turbo/libjpeg-turbo.git
    GIT_TAG 3.0.4
    SOURCE_DIR ${CMAKE_BINARY_DIR}/libjpeg-turbo-src
    BINARY_DIR ${CMAKE_BINARY_DIR}/libjpeg-turbo-build
    INSTALL_DIR ${CMAKE_BINARY_DIR}/libjpeg-turbo-install
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DENABLE_SHARED=ON
      -DENABLE_STATIC=ON
      -DCMAKE_POSITION_INDEPENDENT_CODE=ON

  )
  ExternalProject_Get_Property(libjpeg-turbo install_dir)

  set(LIBJPEG_TURBO_INCLUDE_DIRS ${install_dir}/include)

  if(WIN32)
    set(LIBJPEG_TURBO_LIBRARIES ${install_dir}/lib/turbojpeg-static.lib)
  else()
    set(LIBJPEG_TURBO_LIBRARIES ${install_dir}/lib/libturbojpeg.a)
  endif()

  include_directories(${LIBJPEG_TURBO_INCLUDE_DIRS})

endif()

find_package(OpenGL REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


add_subdirectory(source)
# add_subdirectory(tests)