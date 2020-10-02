project(shiny-runtime)

add_library(shiny-runtime
    Evaluator.cpp
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