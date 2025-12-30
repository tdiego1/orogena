# Local development hooks - NOT checked into version control
# This file runs after CMake configuration completes
#
# To enable: Add to your .gitignore and source from CMakeLists.txt (optional)
# Or use CMakePresets.json (recommended)

# Create symlink for clangd/VSCode
# Ensures compile_commands.json is discoverable at build/compile_commands.json
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(COMPILE_COMMANDS_LINK "${CMAKE_SOURCE_DIR}/build/compile_commands.json")

    # Create build directory if it doesn't exist
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

    # Get relative path from build/ to the actual compile_commands.json
    file(RELATIVE_PATH SYMLINK_TARGET
        "${CMAKE_SOURCE_DIR}/build"
        "${CMAKE_BINARY_DIR}/compile_commands.json"
    )

    # Create symlink (cross-platform compatible)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${SYMLINK_TARGET}"
            "${COMPILE_COMMANDS_LINK}"
        RESULT_VARIABLE SYMLINK_RESULT
    )

    if(SYMLINK_RESULT EQUAL 0)
        message(STATUS "IDE Helper: Symlink created at build/compile_commands.json")
    endif()
endif()
