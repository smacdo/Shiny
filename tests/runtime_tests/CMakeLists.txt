project(test-shiny-runtime)

add_executable(test-shiny-runtime
    TestMain.cpp
    CharacterStreamTests.cpp
    ExceptionTests.cpp
    ReaderTests.cpp
)

set_target_properties(test-shiny-runtime
    PROPERTIES
        CXX_EXTENSIONS OFF
)

target_compile_features(test-shiny-runtime
    PUBLIC
        cxx_std_17
)

target_link_libraries(test-shiny-runtime
    PRIVATE
        Catch2::Catch2
        shiny-runtime
        spdlog
)

if(MSVC)
  target_compile_options(test-shiny-runtime
    PRIVATE
        /WX)
else()
  target_compile_options(test-shiny-runtime
    PRIVATE
        -Werror)
endif()
