cmake_minimum_required(VERSION 3.15)
project(musiC++)

SET(CMAKE_CXX_FLAGS -pthread)
SET(CMAKE_CXX_FLAGS -lncurses)
set(CMAKE_CXX_STANDARD 14)

add_executable(musiC++ main.cpp)