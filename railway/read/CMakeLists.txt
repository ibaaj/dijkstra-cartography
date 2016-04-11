cmake_minimum_required(VERSION 3.1)
project(read_railway)

find_package(CURL REQUIRED)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lprotobuf-lite -losmpbf -lz -lcurl -lboost_serialization")

set(SOURCE_FILES main.cpp)
add_executable(read_railway ${SOURCE_FILES} osmpbfreader.h json.hpp)