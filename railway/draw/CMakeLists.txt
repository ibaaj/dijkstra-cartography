cmake_minimum_required(VERSION 3.1)
project(drawing_railway)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -framework PROJ -lboost_serialization -I/usr/local/include/cairo -L/usr/local/lib/ -lcairo")


set(SOURCE_FILES main.cpp)
add_executable(drawing_railway ${SOURCE_FILES})