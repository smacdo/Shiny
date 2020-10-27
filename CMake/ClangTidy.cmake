# Author: Scott MacDonald
# CMake module for C++ ASAN/TSAN/UBSAN.
# -----------------------------------------------------------------------------
# NOTE: Only use one sanitizer per build output. Results are best when all
#       targets are built with the sanitizer. Do not mix and match.
find_program(
    CLANG_TIDY_EXE NAMES "clang-tidy"
    DOC "Path to clang-tidy executable")

if(CLANG_TIDY_EXE)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
    set(HAS_CLANG_TIDY)
    set(CLANG_TIDY_PROPERTIES "${CLANG_TIDY_EXE}" "-checks=*,-clang-analyzer-cplusplus")
else()
  message(STATUS "clang-tidy NOT found!")
  set(CLANG_TIDY_PROPERTIES "" CACHE STRING "" FORCE) # clear the flag
endif()

if(HAS_CLANG_TIDY)
  option(USE_CLANG_TIDY "Run clang tidy on supported targets" ON)
endif()

function(target_use_clang_tidy target_name)
  if(USE_CLANG_TIDY)
      message(STATUS "Using clang-tidy on ${target_name}")
      set_target_properties(
        "${target_name}"
        PROPERTIES
          CXX_CLANG_TIDY "${CLANG_TIDY_PROPERTIES}"
      )
  endif()
endfunction()