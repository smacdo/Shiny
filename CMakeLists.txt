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

# Only enable tests when this project is building itself. That way when other
# projects include this CMake file it won't build our tests too.
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    enable_testing()
endif()

# If tests are enabled pull in the unit test projects.
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  add_subdirectory(tests/runtime_tests) 
endif()
