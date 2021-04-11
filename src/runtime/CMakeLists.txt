project(shiny-runtime
    DESCRIPTION "Shiny VM runtime"
    HOMEPAGE_URL "https://github.com/smacdo/Shiny/"
    VERSION 0.2)

add_library(shiny-runtime
    allocators/FreeListAllocator.cpp
    allocators/MallocAllocator.cpp
    procedures/BooleanProcedures.cpp
    procedures/ListProcedures.cpp
    procedures/MathProcedures.cpp
    procedures/PairProcedures.cpp
    procedures/SymbolProcedures.cpp
    Allocator.cpp
    CharacterStream.cpp
    EnvironmentFrame.cpp
    Evaluator.cpp
    Exception.cpp
    Procedures.cpp
    RawString.cpp
    RuntimeApi.cpp
    Reader.cpp
    Value.cpp
    VmState.cpp
)

target_compile_features(shiny-runtime
    PUBLIC
        cxx_std_17
)

target_include_directories(shiny-runtime
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(shiny-runtime
    PRIVATE
        fmt
        spdlog
)

#target_use_clang_tidy(shiny-runtime)
target_use_sanitizers(shiny-runtime)

# TODO: Move to a function.
if(MSVC)
  target_compile_options(shiny-runtime
    PRIVATE
        /W4 /WX)
else()
  target_compile_options(shiny-runtime
    PRIVATE
        -Wall -Wextra -pedantic -Werror)
endif()

# Installation rules.
install(
    TARGETS shiny-runtime
    EXPORT shiny-runtime-targets
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ShinyRuntimeConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_file(ShinyRuntimeConfig.cmake.in ShinyRuntimeConfig.cmake @ONLY)
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/ShinyRuntimeConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/ShinyRuntimeConfigVersion.cmake"
    DESTINATION lib/cmake/ShinyRuntime
)