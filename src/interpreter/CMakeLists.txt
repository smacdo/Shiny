add_executable(shiny-interpreter
    InteractiveSession.cpp
    ShinyInterpreter.cpp
)

target_link_libraries(shiny-interpreter
    PRIVATE
        shiny-runtime
)
