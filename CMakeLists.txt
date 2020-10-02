cmake_minimum_required(VERSION 3.15)
project(shiny)

add_subdirectory(thirdparty)
add_subdirectory(src/interpreter)
add_subdirectory(src/runtime)
add_subdirectory(tests/runtime_tests)