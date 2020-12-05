project(shiny-interpreter
    DESCRIPTION "Shiny REPL"
    HOMEPAGE_URL "https://github.com/smacdo/Shiny/"
    VERSION 0.2)

add_executable(shiny-interpreter
    InteractiveSession.cpp
    ShinyInterpreter.cpp
)

# Add a custom "repl" target to easily run the REPL part of this program.
add_custom_target(repl
  COMMAND $<TARGET_FILE:shiny-interpreter>)

target_link_libraries(shiny-interpreter
    PRIVATE
        shiny-runtime
)

#target_use_clang_tidy(shiny-interpreter)
target_use_sanitizers(shiny-interpreter)

if(MSVC)
  target_compile_options(shiny-interpreter
    PRIVATE
        /W4 /WX)
else()
  target_compile_options(shiny-interpreter
    PRIVATE
        -Wall -Wextra -pedantic -Werror)
endif()

# Installation rules.
install(
    TARGETS shiny-interpreter
    EXPORT shiny-interpreter-targets
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ShinyConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_file(ShinyConfig.cmake.in ShinyConfig.cmake @ONLY)
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/ShinyConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/ShinyConfigVersion.cmake"
    DESTINATION lib/cmake/Shiny
)
