#!/bin/bash
# Orogena vcpkg setup script
# This script sets up vcpkg for the project

set -e

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

VCPKG_DIR="${VCPKG_ROOT:-$HOME/vcpkg}"

echo -e "${BLUE}==> Setting up vcpkg for Orogena${NC}"

# Check if vcpkg already exists
if [ -d "$VCPKG_DIR" ]; then
    echo -e "${GREEN}==> vcpkg already installed at: $VCPKG_DIR${NC}"

    # Update vcpkg
    echo -e "${BLUE}==> Updating vcpkg...${NC}"
    cd "$VCPKG_DIR"
    git pull
    ./bootstrap-vcpkg.sh -disableMetrics
else
    echo -e "${BLUE}==> Cloning vcpkg to: $VCPKG_DIR${NC}"
    git clone https://github.com/microsoft/vcpkg.git "$VCPKG_DIR"

    echo -e "${BLUE}==> Bootstrapping vcpkg...${NC}"
    cd "$VCPKG_DIR"
    ./bootstrap-vcpkg.sh -disableMetrics
fi

# Export VCPKG_ROOT
echo -e "${BLUE}==> Setting VCPKG_ROOT environment variable${NC}"
export VCPKG_ROOT="$VCPKG_DIR"

# Add to shell config if not already there
SHELL_CONFIG=""
RELOAD_CMD=""

# Detect shell
if [ -n "$FISH_VERSION" ] || [[ "$SHELL" == *"fish"* ]]; then
    # Fish shell
    SHELL_CONFIG="$HOME/.config/fish/config.fish"
    RELOAD_CMD="source $SHELL_CONFIG"
    mkdir -p "$HOME/.config/fish"

    if ! grep -q "VCPKG_ROOT" "$SHELL_CONFIG" 2>/dev/null; then
        echo -e "${BLUE}==> Adding VCPKG_ROOT to $SHELL_CONFIG${NC}"
        echo "" >> "$SHELL_CONFIG"
        echo "# vcpkg configuration" >> "$SHELL_CONFIG"
        echo "set -gx VCPKG_ROOT \"$VCPKG_DIR\"" >> "$SHELL_CONFIG"
        echo -e "${YELLOW}==> Please run: $RELOAD_CMD${NC}"
    else
        echo -e "${GREEN}==> VCPKG_ROOT already in $SHELL_CONFIG${NC}"
    fi
elif [ -f "$HOME/.bashrc" ]; then
    # Bash shell
    SHELL_CONFIG="$HOME/.bashrc"
    RELOAD_CMD="source $SHELL_CONFIG"

    if ! grep -q "VCPKG_ROOT" "$SHELL_CONFIG"; then
        echo -e "${BLUE}==> Adding VCPKG_ROOT to $SHELL_CONFIG${NC}"
        echo "" >> "$SHELL_CONFIG"
        echo "# vcpkg configuration" >> "$SHELL_CONFIG"
        echo "export VCPKG_ROOT=\"$VCPKG_DIR\"" >> "$SHELL_CONFIG"
        echo -e "${YELLOW}==> Please run: $RELOAD_CMD${NC}"
    else
        echo -e "${GREEN}==> VCPKG_ROOT already in $SHELL_CONFIG${NC}"
    fi
elif [ -f "$HOME/.zshrc" ]; then
    # Zsh shell
    SHELL_CONFIG="$HOME/.zshrc"
    RELOAD_CMD="source $SHELL_CONFIG"

    if ! grep -q "VCPKG_ROOT" "$SHELL_CONFIG"; then
        echo -e "${BLUE}==> Adding VCPKG_ROOT to $SHELL_CONFIG${NC}"
        echo "" >> "$SHELL_CONFIG"
        echo "# vcpkg configuration" >> "$SHELL_CONFIG"
        echo "export VCPKG_ROOT=\"$VCPKG_DIR\"" >> "$SHELL_CONFIG"
        echo -e "${YELLOW}==> Please run: $RELOAD_CMD${NC}"
    else
        echo -e "${GREEN}==> VCPKG_ROOT already in $SHELL_CONFIG${NC}"
    fi
fi

echo -e "${GREEN}==> vcpkg setup complete!${NC}"
echo -e "${BLUE}==> vcpkg location: $VCPKG_DIR${NC}"
echo ""
echo -e "${BLUE}Next steps:${NC}"
echo -e "  1. Run: ${YELLOW}source $SHELL_CONFIG${NC} (or restart terminal)"
echo -e "  2. Build Orogena: ${YELLOW}cmake --workflow --preset debug${NC}"
