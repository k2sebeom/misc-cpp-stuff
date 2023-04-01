# Minimum Version for CMake
cmake_minimum_required(VERSION 3.11)

# Project Info
project(
  LearnCpp
  VERSION 0.1
  DESCRIPTION "Example Project for learing Cpp Compile Structure"
  LANGUAGES C CXX)

include(FetchContent)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -D__MACOSX_CORE__")

FetchContent_Declare(
  SndFile
  GIT_REPOSITORY "https://github.com/libsndfile/libsndfile.git"
  GIT_TAG "1.1.0"
)

# FetchContent_Declare(
#   RtAudio
#   GIT_REPOSITORY "https://github.com/thestk/rtaudio.git"
#   GIT_TAG "5.2.0"
# )

FetchContent_MakeAvailable(SndFile)

# FetchContent_MakeAvailable(RtAudio)
# add_executable (program main.cc)
# target_compile_options(program PUBLIC -Wall -Werror)

add_executable(wire rt.cc RtAudio.cpp)
target_compile_options(wire PUBLIC -Wall -Werror)
target_link_libraries(wire PRIVATE "-framework CoreAudio" "-framework CoreFoundation" sndfile)

add_executable(decode decode.cc)
target_compile_options(decode PUBLIC -Wall -Werror)
target_include_directories(decode PRIVATE /opt/homebrew/Cellar/ffmpeg/5.1.2/include)
target_link_directories(decode PRIVATE /opt/homebrew/Cellar/ffmpeg/5.1.2/lib)
target_link_libraries(decode PRIVATE
  avcodec
  avformat
  avfilter
  avdevice
  swresample
  swscale
  avutil
)

add_executable(safety safety.cc)
# target_link_libraries(safety PUBLIC pthread)

add_executable(encode encode.cc)
target_compile_options(encode PUBLIC -Wall -Werror)
target_include_directories(encode PRIVATE /opt/homebrew/Cellar/ffmpeg/5.1.2/include)
target_link_directories(encode PRIVATE /opt/homebrew/Cellar/ffmpeg/5.1.2/lib)
target_link_libraries(encode PRIVATE
  avcodec
  avformat
  avfilter
  avdevice
  swresample
  swscale
  avutil
  sndfile
)


add_executable(transcode transcode.cc)
target_compile_options(transcode PUBLIC -Wall -Werror)
target_include_directories(transcode PRIVATE /opt/homebrew/Cellar/ffmpeg/5.1.2/include)
target_link_directories(transcode PRIVATE /opt/homebrew/Cellar/ffmpeg/5.1.2/lib)
target_link_libraries(transcode PRIVATE
  avcodec
  avformat
  avfilter
  avdevice
  swresample
  swscale
  avutil
  sndfile
)