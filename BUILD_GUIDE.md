# Orogena Build Guide

This guide explains the CMake Presets-based build system for Orogena.

## Prerequisites

- **Compiler**: Clang 21+ (configured as default)
- **CMake**: 3.25+ (for CMakePresets.json support)
- **vcpkg**: For dependency management
- **Qt**: 6.8+
- **Ninja**: Build system (faster than Make)

## Build Presets Overview

Orogena uses CMake Presets for streamlined configuration. Each preset configures the project for a specific use case:

| Preset | Build Type | Tests | LTO | Use Case |
|--------|-----------|-------|-----|----------|
| **debug** | Debug | ✓ | ✗ | Daily development, debugging |
| **dev** | Debug | ✓ | ✗ | IDE default (same as debug) |
| **release** | Release | ✗ | ✓ | Production builds |
| **relwithdebinfo** | RelWithDebInfo | ✓ | ✗ | Profiling and optimization |
| **ci** | Release | ✓ | ✗ | Continuous integration |

## Quick Start

### One-Step Workflow (Recommended)

The fastest way to build and test:

```bash
# Configure, build, and test in one command
cmake --workflow --preset debug
```

This runs the complete workflow:
1. Configure project with debug preset
2. Build all targets
3. Run all tests

### Step-by-Step Build

If you prefer manual control:

```bash
# 1. Configure
cmake --preset debug

# 2. Build
cmake --build --preset debug

# 3. Run tests
ctest --preset debug

# 4. Run application
./build/debug/src/orogena
```

### Using Helper Script

```bash
./scripts/configure.sh debug    # Configure with preset
cmake --build --preset debug    # Build
```

## Common Workflows

### Development Workflow

For daily development work:

```bash
# Initial setup (once)
cmake --workflow --preset dev

# After making changes
cmake --build --preset dev              # Incremental build
ctest --preset dev                      # Run tests
./build/dev/src/orogena                 # Test your changes

# Run specific test
./build/dev/tests/test_unit --gtest_filter=MyTest.*
```

### Release Build

For production builds with maximum performance:

```bash
# Full optimization with LTO
cmake --workflow --preset release

# Run release binary
./build/release/src/orogena
```

**Note**: Release builds take longer due to Link-Time Optimization but run 10-20% faster.

### Profiling Build

For performance analysis with debug symbols:

```bash
# Build with optimization + symbols
cmake --preset relwithdebinfo
cmake --build --preset relwithdebinfo

# Profile with perf
perf record -g ./build/relwithdebinfo/src/orogena
perf report

# Or use valgrind
valgrind --tool=callgrind ./build/relwithdebinfo/src/orogena
kcachegrind callgrind.out.*
```

### CI/CD Pipeline

For continuous integration:

```bash
# CI preset treats warnings as errors
cmake --workflow --preset ci
```

## Build Directory Structure

Each preset creates its own build directory:

```
build/
├── debug/              # Debug build output
│   ├── compile_commands.json
│   ├── src/orogena
│   └── tests/test_unit
├── release/            # Release build output
├── relwithdebinfo/     # Profiling build output
├── dev/                # Development build (IDE default)
└── ci/                 # CI build output
```

**Benefits**:
- Switch between configurations without rebuilding
- Parallel builds for different configs
- Isolated build artifacts

## Advanced Usage

### Listing Available Presets

```bash
# List all configure presets
cmake --list-presets

# List build presets
cmake --build --list-presets

# List test presets
ctest --list-presets
```

### Verbose Build Output

```bash
# See full compiler commands
cmake --build --preset debug --verbose

# Or use verbose build preset
cmake --build --preset debug-verbose
```

### Clean Build

```bash
# Clean specific preset
rm -rf build/debug
cmake --workflow --preset debug

# Clean all builds
rm -rf build
cmake --workflow --preset debug
```

### Parallel Builds

Presets automatically use all CPU cores (`jobs: 0` means auto-detect). To limit:

```bash
# Limit to 4 cores
cmake --build --preset debug -j 4
```

## IDE Integration

### VSCode

The project includes [.vscode/settings.json](.vscode/settings.json) with preset configuration:

1. **Default preset**: `dev` (debug build with tests)
2. **CMake Tools** extension will automatically detect presets
3. Use the CMake sidebar to select presets

**VSCode Keyboard Shortcuts**:
- `Ctrl+Shift+P` → "CMake: Select Configure Preset"
- `F7` → Build
- `Ctrl+F5` → Run without debugging

### CLion

CLion 2023.3+ has native CMake Presets support:

1. **File → Settings → Build, Execution, Deployment → CMake**
2. CLion will auto-detect presets from `CMakePresets.json`
3. Select preset from dropdown in toolbar

### Qt Creator

Qt Creator 11+ supports CMake Presets:

1. **Projects → Build Settings**
2. CMake presets appear in configuration dropdown
3. Select preset and build normally

## Compiler Configuration

### Clang-Specific Optimizations

The project is configured for **Clang 21+** with:

**Debug mode** (`-O0 -g`):
- No optimization for fast compilation
- Full debug symbols
- Frame pointers for better stack traces

**Release mode** (`-O3 -march=native -flto=thin`):
- Aggressive optimization
- CPU-specific instructions (not portable!)
- Thin LTO for cross-module optimization

**Warning flags** (all modes):
```
-Wall -Wextra -Wpedantic -Wconversion -Wshadow
```

### Overriding Compiler

To use a different compiler (not recommended):

```bash
# Set before first configure
export CC=gcc
export CXX=g++
cmake --preset debug

# Or edit CMakePresets.json:
"CMAKE_C_COMPILER": "gcc",
"CMAKE_CXX_COMPILER": "g++"
```

## Testing

### Running All Tests

```bash
# Quiet mode (default)
ctest --preset debug

# Verbose output
ctest --preset dev

# Show output only on failure
ctest --preset debug --output-on-failure
```

### Running Specific Tests

```bash
# By name
ctest --preset debug -R PlatePhysicsTest

# By number
ctest --preset debug -I 3,5  # Run tests 3-5

# Exclude tests
ctest --preset debug -E SlowTest
```

### Test Output

```bash
# Run test binary directly for detailed output
./build/debug/tests/test_unit

# With Google Test filters
./build/debug/tests/test_unit --gtest_filter=PlatePhysicsTest.*

# List available tests
./build/debug/tests/test_unit --gtest_list_tests
```

## Code Quality Tools

### Format Code

```bash
# Format all source files
clang-format -i $(find src -name "*.cpp" -o -name "*.h")

# Check formatting without modifying
clang-format --dry-run --Werror src/**/*.{cpp,h}
```

### Static Analysis

```bash
# Run clang-tidy on all files
clang-tidy src/**/*.cpp -- -std=c++20

# With compile commands from build
clang-tidy -p build/debug src/core/*.cpp

# Auto-fix issues (careful!)
clang-tidy -p build/debug --fix src/core/*.cpp
```

## Troubleshooting

### "CMake version too old"

```bash
# Check version
cmake --version

# On Arch/CachyOS
sudo pacman -S cmake

# Or install from Kitware
pip install cmake --upgrade
```

### "Preset not found"

Ensure you're running CMake 3.25+:
```bash
cmake --version  # Must be >= 3.25
```

### "vcpkg not found"

```bash
# Set VCPKG_ROOT
export VCPKG_ROOT=$HOME/vcpkg

# Add to ~/.bashrc or ~/.zshrc
echo 'export VCPKG_ROOT=$HOME/vcpkg' >> ~/.bashrc
```

### "Ninja not found"

```bash
# Install Ninja
sudo pacman -S ninja        # Arch/CachyOS
sudo apt install ninja-build  # Ubuntu/Debian
```

### Build directory mismatch

If switching from legacy builds to presets:

```bash
# Clean old build directory
rm -rf build

# Reconfigure with preset
cmake --workflow --preset debug
```

### Compilation errors after switching presets

Different presets may expose different warnings/errors:

```bash
# CI preset treats warnings as errors
# If failing, fix warnings first:
cmake --preset debug  # More lenient
```

## Performance Tips

### Faster Incremental Builds

1. **Use ccache** (compiler cache):
```bash
sudo pacman -S ccache
export CMAKE_CXX_COMPILER_LAUNCHER=ccache
```

2. **Use mold linker** (faster than ld):
```bash
sudo pacman -S mold
# Add to CMakeLists.txt:
# add_link_options(-fuse-ld=mold)
```

3. **Reduce parallel jobs** if running out of memory:
```bash
cmake --build --preset debug -j 4  # Instead of auto
```

### Faster Clean Builds

```bash
# Use sccache for distributed caching (optional)
cargo install sccache
export CMAKE_CXX_COMPILER_LAUNCHER=sccache
```

## Reference

### Environment Variables

| Variable | Purpose | Example |
|----------|---------|---------|
| `VCPKG_ROOT` | vcpkg installation path | `$HOME/vcpkg` |
| `CMAKE_PREFIX_PATH` | Qt installation path | `/opt/qt6` |
| `CC` | C compiler override | `clang` |
| `CXX` | C++ compiler override | `clang++` |

### File Locations

| File | Purpose |
|------|---------|
| [CMakePresets.json](CMakePresets.json) | Preset definitions |
| [CMakeLists.txt](CMakeLists.txt) | Main build configuration |
| [.clang-format](.clang-format) | Code formatting rules |
| [.clang-tidy](.clang-tidy) | Static analysis rules |
| [vcpkg.json](vcpkg.json) | Dependency manifest |

---

**Last Updated**: December 2025
