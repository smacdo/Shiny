add_executable(shiny-interpreter
    InteractiveSession.cpp
    ShinyInterpreter.cpp
)

target_link_libraries(shiny-interpreter
    PRIVATE
        shiny-runtime
)

if(MSVC)
  target_compile_options(shiny-interpreter
    PRIVATE
        /W4 /WX)
else()
  target_compile_options(shiny-interpreter
    PRIVATE
        -Wall -Wextra -pedantic -Werror)
endif()