if(CMAKE_EXPORT_COMPILE_COMMANDS)
  add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/build/compile_commands.json
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/build/compile_commands.json
    DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
    COMMENT "Link compile_commands.json to build/"
  )

  add_custom_target(link_compile_commands_json ALL
    DEPENDS ${CMAKE_SOURCE_DIR}/build/compile_commands.json
  )
endif()

