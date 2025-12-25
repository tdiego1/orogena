#!/bin/bash
# Build script for Orogena

set -e  # Exit on error

echo "Building Orogena..."

# Check if vcpkg is configured
if [ -z "$VCPKG_ROOT" ]; then
    echo "Warning: VCPKG_ROOT not set. Using default location..."
    VCPKG_ROOT="$HOME/vcpkg"
fi

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
    -DOROGENA_BUILD_TESTS=ON

# Build
cmake --build . -j$(nproc)

echo "Build complete! Binary: build/src/orogena"
