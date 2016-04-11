cmake_minimum_required(VERSION 3.1)
project(read_amazon)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -lprotobuf-lite -losmpbf -lz ")

set(SOURCE_FILES main.cpp)
add_executable(read_amazon ${SOURCE_FILES} osmpbfreader.h)