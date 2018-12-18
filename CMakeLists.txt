cmake_minimum_required(VERSION 3.13)
project(Citation_graph)

set(CMAKE_CXX_STANDARD 17)

add_executable(Citation_graph src/main.cpp src/citation_graph.h)
add_executable(Test test/test.cc src/citation_graph.h)
add_executable(Example zadanie/citation_graph_example.cc src/citation_graph.h)
add_executable(Test2 test/citation_graph_memory_testing.h test/citation_graph_unit_tests.cc src/citation_graph.h)
add_executable(Test3 test/elo.cpp src/citation_graph.h)