cmake_minimum_required(VERSION 3.8)
project(word_segmentation)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp Limonp/Logger.hpp Limonp/StdExtension.hpp Limonp/StringUtil.hpp unicode.h)
add_executable(word_segmentation ${SOURCE_FILES})