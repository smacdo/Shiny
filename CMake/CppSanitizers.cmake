# Author: Scott MacDonald
# CMake module for C++ ASAN/TSAN/UBSAN.
# -----------------------------------------------------------------------------
# NOTE: Only use one sanitizer per build output. Results are best when all
#       targets are built with the sanitizer. Do not mix and match.
option(USE_ADDRESS_SANITIZER "Address sanitizer" OFF)
option(USE_MEMORY_SANITIZER "Memory sanitizer" OFF)
option(USE_UNDEF_SANITIZER "Undefined behavior sanitizer" OFF)

# Instrument C++ target with address sanitizer
function(target_use_sanitizers target_name)
    if (USE_ADDRESS_SANITIZER)
      if (USE_UNDEF_SANITIZER)
        # ASAN and UBSAN
        message(STATUS "Enabled ASAN,UBSAN for ${target_name}")

        target_compile_options(
          ${target_name}
            PUBLIC
              -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_libraries(
          ${target_name}
            PUBLIC
              -fsanitize=address,undefined)
      else()
        # ASAN only.
        message(STATUS "Enabled ASAN for ${target_name}")

        target_compile_options(
          ${target_name}
            PUBLIC
              -fsanitize=address -fno-omit-frame-pointer)
        target_link_libraries(
          ${target_name}
            PUBLIC
              -fsanitize=address)
      endif()
    elseif(USE_MEMORY_SANITIZER)
      if (USE_UNDEF_SANITIZER)
          # MSAN and UBSAN
          message(STATUS "Enabled MSAN,UBSAN for ${target_name}")

          target_compile_options(
            ${target_name}
              PUBLIC
                -fsanitize=memory,undefined -fno-omit-frame-pointer)
          target_link_libraries(
            ${target_name}
              PUBLIC
                -fsanitize=memory,undefined)
        else()
          # ASAN only.
          message(STATUS "Enabled MSAN for ${target_name}")

          target_compile_options(
            ${target_name}
              PUBLIC
                -fsanitize=memory -fno-omit-frame-pointer)
          target_link_libraries(
            ${target_name}
              PUBLIC
                -fsanitize=memory)
        endif()
    endif()
endfunction()