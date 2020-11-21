cmake_minimum_required(VERSION 3.15)
project(shiny
  VERSION 0.1.0
  DESCRIPTION "An experimental Scheme R7RS interpreter"
  HOMEPAGE_URL "https://github.com/smacdo/Shiny"
)

set(
  CMAKE_MODULE_PATH
    "${PROJECT_SOURCE_DIR}/CMake"
    ${CMAKE_MODULE_PATH}
)

include(ClangTidy)
include(CppSanitizers)

add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

add_subdirectory(thirdparty)
add_subdirectory(src/interpreter)
add_subdirectory(src/runtime)
add_subdirectory(tests/runtime_tests)