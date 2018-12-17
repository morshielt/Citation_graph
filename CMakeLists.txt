cmake_minimum_required(VERSION 3.13)
project(Citation_graph)

set(CMAKE_CXX_STANDARD 17)

add_executable(Citation_graph src/main.cpp src/citation_graph.h)
add_executable(Test src/test.cc src/citation_graph.h)