project(test-shiny-runtime)

add_executable(test-shiny-runtime
    TestMain.cpp
    testset1.cpp
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