add_executable(shiny-interpreter
    InteractiveSession.cpp
    ShinyInterpreter.cpp
)

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