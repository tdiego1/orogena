#!/bin/bash
# Orogena build configuration script
# Usage: ./scripts/configure.sh [preset]
# Available presets: debug (default), release, relwithdebinfo, dev, ci

set -e

PRESET="${1:-debug}"

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${BLUE}==> Configuring Orogena with preset: ${PRESET}${NC}"

# Check if vcpkg is available
if [ -z "$VCPKG_ROOT" ]; then
    echo -e "${YELLOW}Warning: VCPKG_ROOT not set. Trying default location...${NC}"
    if [ -d "$HOME/vcpkg" ]; then
        export VCPKG_ROOT="$HOME/vcpkg"
    else
        echo "Error: vcpkg not found. Please set VCPKG_ROOT environment variable."
        exit 1
    fi
fi

# Configure with CMake using presets
cmake --preset="$PRESET"

echo -e "${GREEN}==> Configuration complete!${NC}"
echo -e "${BLUE}==> Build with: cmake --build --preset $PRESET${NC}"
echo -e "${BLUE}==> Or use workflow: cmake --workflow --preset $PRESET${NC}"
echo -e "${BLUE}==> Run tests with: ctest --preset $PRESET${NC}"
