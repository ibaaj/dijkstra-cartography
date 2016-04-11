cmake_minimum_required(VERSION 3.1)
project(read_flights)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework PROJ  -std=c++11 -I/usr/local/include/cairo -L/usr/local/lib/ -lcairo")

set(SOURCE_FILES main.cpp)
add_executable(read_flights ${SOURCE_FILES})