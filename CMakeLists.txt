cmake_minimum_required(VERSION 3.20)

message("VCPKG_ROOT from environment: $ENV{VCPKG_ROOT}")

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
        add_compile_options(-O3 -march=native -flto=thin)
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

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Subdirectories
add_subdirectory(src)

if(OROGENA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation rules (for future)
# install(TARGETS orogena DESTINATION bin)
