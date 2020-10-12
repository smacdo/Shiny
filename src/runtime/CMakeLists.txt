project(shiny-runtime)

add_library(shiny-runtime
    Allocator.cpp
    CharacterStream.cpp
    FreeListAllocator.cpp
    Evaluator.cpp
    Exception.cpp
    Reader.cpp
    Value.cpp
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

