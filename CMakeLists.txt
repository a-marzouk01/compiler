cmake_minimum_required(VERSION 3.13)
project(compiler)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp)

add_executable(compiler ${SOURCE_FILES})
