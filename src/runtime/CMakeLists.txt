project(shiny-runtime)

add_library(shiny-runtime
    Evaluator.cpp
    FixnumObject.cpp
    Object.cpp
    Reader.cpp
)

target_include_directories(shiny-runtime
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
