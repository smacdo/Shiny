project(shiny-runtime)

add_library(shiny-runtime
    CharacterStream.cpp
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

if(MSVC)
  target_compile_options(shiny-runtime
    PRIVATE
        /W4 /WX)
else()
  target_compile_options(shiny-runtime
    PRIVATE
        -Wall -Wextra -pedantic -Werror)
endif()
