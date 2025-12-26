# Orogena Setup Guide

Quick guide to get Orogena building on your system.

## Step 1: Install System Dependencies

### Arch Linux / CachyOS
```bash
sudo pacman -S cmake clang ninja qt6-base qt6-openglwidgets mesa git
```

### Ubuntu / Debian
```bash
sudo apt install cmake clang ninja-build qt6-base-dev libgl-dev git
```

### macOS
```bash
brew install cmake llvm ninja qt@6 git
```

### Windows
- Install [Visual Studio 2022](https://visualstudio.microsoft.com/) with C++ development tools
- Install [CMake](https://cmake.org/download/)
- Install [Git](https://git-scm.com/download/win)
- Install [Qt 6.8+](https://www.qt.io/download)

## Step 2: Setup vcpkg

vcpkg manages C++ dependencies (spdlog, glm, nlohmann_json, etc.).

### Automatic Setup (Recommended)
```bash
cd orogena
./scripts/setup_vcpkg.sh
```

This will:
1. Clone vcpkg to `~/vcpkg`
2. Bootstrap vcpkg
3. Add `VCPKG_ROOT` to your shell configuration

Then reload your shell:
```bash
source ~/.bashrc  # or ~/.zshrc
```

### Manual Setup
```bash
# Clone vcpkg
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg

# Bootstrap
cd ~/vcpkg
./bootstrap-vcpkg.sh -disableMetrics

# Set environment variable
echo 'export VCPKG_ROOT="$HOME/vcpkg"' >> ~/.bashrc
source ~/.bashrc
```

## Step 3: Verify Environment

Check that everything is set up correctly:

```bash
# Check compiler
clang++ --version  # Should be 10+, preferably 21+

# Check CMake
cmake --version    # Should be 3.25+

# Check vcpkg
echo $VCPKG_ROOT   # Should print vcpkg path (e.g., /home/user/vcpkg)
ls $VCPKG_ROOT     # Should show vcpkg files
```

## Step 4: Build Orogena

### Quick Start (One Command)
```bash
cd orogena
cmake --workflow --preset debug
```

This will:
1. Configure the project with vcpkg
2. Download and build all dependencies
3. Build Orogena
4. Run tests

**Note**: First build takes 10-30 minutes to compile dependencies. Subsequent builds are much faster.

### Step-by-Step Build
```bash
# Configure
cmake --preset debug

# Build (incremental)
cmake --build --preset debug

# Run tests
ctest --preset debug

# Run application
./build/debug/src/orogena
```

## Step 5: IDE Setup (Optional)

### VSCode
1. Install extensions:
   - **C/C++** (`ms-vscode.cpptools`) OR **clangd** (`llvm-vs-code-extensions.vscode-clangd`)
   - **CMake Tools** (`ms-vscode.cmake-tools`)
   - **clang-format** (`xaver.clang-format`)

2. Open project folder in VSCode:
   ```bash
   code orogena
   ```

3. VSCode will auto-detect CMake presets
4. Select preset from CMake sidebar (default: `dev`)
5. Press `F7` to build

### CLion
1. Open project directory
2. CLion auto-detects `CMakePresets.json`
3. Select preset from toolbar dropdown
4. Build normally

### Qt Creator
1. Open `CMakeLists.txt` as project
2. Qt Creator detects presets
3. Select preset in Build Settings
4. Build normally

## Troubleshooting

### "VCPKG_ROOT not set" error
```bash
# Check if set
echo $VCPKG_ROOT

# If empty, set it manually
export VCPKG_ROOT=$HOME/vcpkg

# Add to shell config permanently
echo 'export VCPKG_ROOT="$HOME/vcpkg"' >> ~/.bashrc
source ~/.bashrc
```

### "Could not find toolchain file" error
This means `VCPKG_ROOT` is not set or pointing to wrong location.

**Solution**:
```bash
# Find where vcpkg is installed
find ~ -maxdepth 3 -name vcpkg -type d

# Set VCPKG_ROOT to that location
export VCPKG_ROOT=/path/to/vcpkg
```

Or create a `CMakeUserPresets.json` with hardcoded path:
```bash
cp CMakeUserPresets.json.example CMakeUserPresets.json
# Edit CMakeUserPresets.json and replace /home/YOUR_USERNAME/vcpkg with actual path
```

### "Qt6 not found" error
**Linux**:
```bash
sudo pacman -S qt6-base qt6-openglwidgets  # Arch
sudo apt install qt6-base-dev              # Ubuntu
```

**macOS**:
```bash
brew install qt@6
export CMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6
```

**Windows**: Install Qt from [qt.io](https://www.qt.io/download)

### "Ninja not found" error
```bash
sudo pacman -S ninja        # Arch
sudo apt install ninja-build  # Ubuntu
brew install ninja          # macOS
```

Or configure with Make instead:
```bash
# Edit CMakePresets.json, change:
"generator": "Ninja"
# to:
"generator": "Unix Makefiles"  # Linux/Mac
# or
"generator": "Visual Studio 17 2022"  # Windows
```

### Build is very slow (first time)
This is normal! vcpkg is compiling dependencies from source:
- spdlog (logging)
- glm (math library)
- nlohmann_json (JSON parsing)
- Qt integration packages

**Progress**: Check vcpkg output - it shows what's being built.

**Speed up**: Use binary caching (advanced):
```bash
export VCPKG_BINARY_SOURCES="clear;default,readwrite"
```

### "Clang not found" error
**Linux**:
```bash
sudo pacman -S clang  # Arch
sudo apt install clang  # Ubuntu
```

**macOS**: Already installed with Xcode Command Line Tools

**Windows**: Install with Visual Studio or LLVM installer

**Alternative**: Use GCC instead (edit `CMakePresets.json`):
```json
"CMAKE_C_COMPILER": "gcc",
"CMAKE_CXX_COMPILER": "g++"
```

## Platform-Specific Notes

### Linux
- Uses system OpenGL drivers
- Qt installed via package manager works best
- clangd works better than Microsoft C++ extension for VSCode

### macOS
- May need to set `CMAKE_PREFIX_PATH` for Qt:
  ```bash
  export CMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6
  ```
- Xcode Command Line Tools required:
  ```bash
  xcode-select --install
  ```

### Windows
- Use "x64 Native Tools Command Prompt for VS 2022"
- Or configure environment in PowerShell:
  ```powershell
  $env:VCPKG_ROOT = "C:\vcpkg"
  ```
- Paths use backslashes in `CMakeUserPresets.json`

## Next Steps

After successful build:

1. **Read documentation**: See [BUILD_GUIDE.md](BUILD_GUIDE.md) for detailed build options
2. **Check coding standards**: See [docs/002_coding_standard.md](docs/002_coding_standard.md)
3. **Understand architecture**: See [docs/003_design_standard.md](docs/003_design_standard.md)
4. **Start developing**: See [CLAUDE.md](CLAUDE.md) for development workflow

## Getting Help

- Check [BUILD_GUIDE.md](BUILD_GUIDE.md) for comprehensive build documentation
- Check [PRESETS_SUMMARY.md](PRESETS_SUMMARY.md) for quick command reference
- File issues on GitHub for build problems
- Check vcpkg documentation: https://vcpkg.io/

---

**Estimated Setup Time**: 10-15 minutes + first build (20-40 minutes)
