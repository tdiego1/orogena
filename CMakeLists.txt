#[[************************************************************************************************/
/**
 * @brief CMake configuration for the Orogena project.
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * For commercial licensing options, please contact: tdiego001@gmail.com
 */
/************************************************************************************************]]#

cmake_minimum_required(VERSION 3.20)

project(Orogena VERSION 1.0.0 LANGUAGES CXX)

# Compiler selection - Use Clang (can be overridden by CMakePresets.json)
if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER clang)
endif()
if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER clang++)
endif()

# C++20 Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler-specific flags for Clang
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Warning flags
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
        -Wconversion
        -Wshadow
    )

    # Release optimizations
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        # Enable LTO for better performance
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
        add_compile_options(-O3)
        if(NOT APPLE)
            # Avoid -march=native on macOS as it can cause cross-compilation issues
            add_compile_options(-march=native)
        endif()
        add_compile_options(-flto=thin)
        add_link_options(-flto=thin)
    endif()

    # Debug optimizations
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-O0 -g -fno-omit-frame-pointer)
    endif()
endif()

# Options
option(OROGENA_BUILD_TESTS "Build unit tests" ON)
option(OROGENA_USE_VULKAN "Use Vulkan instead of OpenGL" OFF)
option(OROGENA_ENABLE_PROFILING "Enable performance profiling" OFF)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL OpenGLWidgets Sql Concurrent)
qt_standard_project_setup()

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find other dependencies (will be configured via vcpkg)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(libassert CONFIG REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Subdirectories
add_subdirectory(src)

if(OROGENA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Shader copying with precise dependency tracking
file(GLOB_RECURSE SHADER_FILES "${CMAKE_SOURCE_DIR}/src/render/shaders/*")

foreach(SHADER_FILE ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME)
    set(OUTPUT_FILE "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME}")

    add_custom_command(
        OUTPUT "${OUTPUT_FILE}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SHADER_FILE}"
            "${OUTPUT_FILE}"
        DEPENDS "${SHADER_FILE}"
        COMMENT "Copying shader: ${SHADER_NAME}"
        VERBATIM
    )

    list(APPEND SHADER_OUTPUTS "${OUTPUT_FILE}")
endforeach()

add_custom_target(copy_shaders ALL DEPENDS ${SHADER_OUTPUTS})

# Copy texture files to build directory
file(GLOB_RECURSE TEXTURE_FILES "${CMAKE_SOURCE_DIR}/resources/textures/*")

foreach(TEXTURE_FILE ${TEXTURE_FILES})
    file(RELATIVE_PATH TEXTURE_REL_PATH "${CMAKE_SOURCE_DIR}/resources/textures" "${TEXTURE_FILE}")
    set(OUTPUT_FILE "${CMAKE_BINARY_DIR}/resources/textures/${TEXTURE_REL_PATH}")

    get_filename_component(OUTPUT_DIR "${OUTPUT_FILE}" DIRECTORY)

    add_custom_command(
        OUTPUT "${OUTPUT_FILE}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${TEXTURE_FILE}"
            "${OUTPUT_FILE}"
        DEPENDS "${TEXTURE_FILE}"
        COMMENT "Copying texture: ${TEXTURE_REL_PATH}"
        VERBATIM
    )

    list(APPEND TEXTURE_OUTPUTS "${OUTPUT_FILE}")
endforeach()

add_custom_target(copy_textures ALL DEPENDS ${TEXTURE_OUTPUTS})

# Copy asset files to build directory
file(GLOB_RECURSE ASSET_FILES "${CMAKE_SOURCE_DIR}/assets/*")

foreach(ASSET_FILE ${ASSET_FILES})
    file(RELATIVE_PATH ASSET_REL_PATH "${CMAKE_SOURCE_DIR}/assets" "${ASSET_FILE}")
    set(OUTPUT_FILE "${CMAKE_BINARY_DIR}/assets/${ASSET_REL_PATH}")

    get_filename_component(OUTPUT_DIR "${OUTPUT_FILE}" DIRECTORY)

    add_custom_command(
        OUTPUT "${OUTPUT_FILE}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${ASSET_FILE}"
            "${OUTPUT_FILE}"
        DEPENDS "${ASSET_FILE}"
        COMMENT "Copying asset: ${ASSET_REL_PATH}"
        VERBATIM
    )

    list(APPEND ASSET_OUTPUTS "${OUTPUT_FILE}")
endforeach()

add_custom_target(copy_assets ALL DEPENDS ${ASSET_OUTPUTS})

add_dependencies(orogena copy_shaders copy_textures copy_assets)

# Installation rules (for future)
# install(TARGETS orogena DESTINATION bin)
# install(DIRECTORY src/render/shaders DESTINATION share/orogena)
