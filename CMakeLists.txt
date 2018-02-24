cmake_minimum_required(VERSION 3.9)
project(aggregate)

set(CMAKE_CXX_STANDARD 17)

# conan stuff
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(candlestick SHARED openclose.c openclose.h)

add_executable(aggregate main.cpp)
target_link_libraries(aggregate ${CONAN_LIBS})