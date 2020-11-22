project(test-shiny-runtime)

add_executable(test-shiny-runtime
    support/TestHelpers.cpp
    support/EvaluatorFixture.cpp
    TestMain.cpp
    CharacterStreamTests.cpp
    FreeListAllocatorTests.cpp
    EnvironmentTests.cpp
    EvaluatorProcedureTests.cpp
    EvaluatorMathTests.cpp
    EvaluatorTests.cpp
    ExceptionTests.cpp
    MallocAllocatorTests.cpp
    RawStringTests.cpp
    ReaderTests.cpp
    RuntimeApiTests.cpp
    SExpressionTests.cpp
    ValueApiTests.cpp
    ValueTests.cpp
    VmStateTests.cpp
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

#target_use_clang_tidy(test-shiny-runtime)
target_use_sanitizers(test-shiny-runtime)

if(MSVC)
  target_compile_options(test-shiny-runtime
    PRIVATE
        /WX)
else()
  target_compile_options(test-shiny-runtime
    PRIVATE
        -Werror)
endif()
