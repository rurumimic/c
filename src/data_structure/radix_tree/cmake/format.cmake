set(FORMAT_STYLE "Google" CACHE STRING "Style for clang-format (Google, LLVM, Mozilla, Chromium, WebKit)")

option(ENABLE_FORMAT "Enable clang-format target" ON)

if(ENABLE_FORMAT)
    find_program(CLANG_FORMAT clang-format)
    if(CLANG_FORMAT)
        add_custom_target(
            clang-format
            COMMAND ${CLANG_FORMAT} -style=${FORMAT_STYLE} -dump-config > .clang-format
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating .clang-format with ${FORMAT_STYLE} style"
            VERBATIM
        )

        add_custom_target(
            format
            COMMAND bash -c "find ${CMAKE_SOURCE_DIR}/src \\( -name \"*.h\" -o -name \"*.c\" -o -name \"*.cpp\" \\) -print0 | xargs -0 -r ${CLANG_FORMAT} -i"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Formatting source files with clang-format"
            VERBATIM
        )
    else()
        message(WARNING "clang-format not found, format target will not be available.")
    endif()
endif()
