# Minimum Version for CMake
cmake_minimum_required(VERSION 3.11)

# Project Info
project(
  LearnCpp
  VERSION 0.1
  DESCRIPTION "Example Project for learing Cpp Compile Structure"
  LANGUAGES C CXX)

include(FetchContent)
FetchContent_Declare(
  Fmt
  GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
  GIT_TAG "9.1.0"
)

FetchContent_Declare(
  PortAudio
  GIT_REPOSITORY "https://github.com/PortAudio/portaudio.git"
  GIT_TAG "v19.7.0"
)

FetchContent_MakeAvailable(Fmt PortAudio)

add_subdirectory(lib)
add_executable (program main.cc)
target_link_libraries(program shape)
target_link_libraries(program portaudio)
target_compile_options(program PUBLIC -Wall -Werror)