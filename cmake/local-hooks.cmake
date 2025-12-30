# Local development hooks for IDE integration
# This file runs after CMake configuration completes
#
# USAGE:
# This hook is designed to be used via CMakeUserPresets.json (git-ignored)
# Copy the example below to your CMakeUserPresets.json:
#
# {
#     "version": 6,
#     "configurePresets": [
#         {
#             "name": "user-dev",
#             "inherits": "dev",
#             "cacheVariables": {
#                 "CMAKE_PROJECT_INCLUDE_BEFORE": "${sourceDir}/cmake/local-hooks.cmake"
#             }
#         }
#     ]
# }
#
# Then configure with: cmake --preset user-dev

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
