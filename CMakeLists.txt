cmake_minimum_required(VERSION 3.10)
project(coloring)

set(CMAKE_CXX_STANDARD 17)

add_executable(coloring main.cpp mmio.cpp Mtx2Graph.cpp)