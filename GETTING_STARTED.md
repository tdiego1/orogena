# Getting Started with Orogena Development

Quick guide to start developing Orogena.

---

## First Time Setup

### 1. Install Dependencies

**Linux (CachyOS/Arch)**:
```bash
sudo pacman -S cmake gcc qt6-base qt6-3d opengl-driver git
```

**Ubuntu/Debian**:
```bash
sudo apt install cmake g++ qt6-base-dev qt6-3d-dev libgl-dev git
```

**macOS**:
```bash
brew install cmake qt@6
```

**Windows**:
- Install Visual Studio 2022 with C++ tools
- Install Qt 6.8 from https://www.qt.io/download
- Install CMake from https://cmake.org/download/

### 2. Install vcpkg

```bash
# Clone vcpkg
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
cd ~/vcpkg

# Bootstrap vcpkg
./bootstrap-vcpkg.sh  # Linux/macOS
# OR
bootstrap-vcpkg.bat   # Windows

# Set environment variable
export VCPKG_ROOT=~/vcpkg  # Add to ~/.bashrc or ~/.zshrc
```

### 3. Clone and Build Orogena

```bash
# Clone the repository
cd ~/projects
git clone <repository-url> orogena
cd orogena

# Build (Linux/macOS)
./scripts/build.sh

# Or manually
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build . -j$(nproc)

# Run
./src/orogena
```

---

## Project Structure Overview

```
src/
├── core/       - Application foundation (Project, Settings, Application)
├── global/     - Scale 1: Plate tectonics simulation
├── regional/   - Scale 2: Regional erosion and tile generation
├── local/      - Scale 3: Fine detail synthesis
├── rendering/  - OpenGL visualization
├── ui/         - Qt user interface
├── database/   - SQLite data persistence
└── utils/      - Logging, threading, helpers
```

**Development flow**: Core → Global → Regional → Local → Rendering → UI

---

## Development Workflow

### Before Writing Code

1. **Read the standards**:
   - [Coding Standard](docs/002_coding_standard.md)
   - [Design Standard](docs/003_design_standard.md)

2. **Check the roadmap**:
   - [Software Development Plan](docs/001_sdp_orogena.md)

3. **Run clang-format**:
   ```bash
   clang-format -i src/**/*.{h,cpp}
   ```

### Writing Code

1. **Create header file** (`src/component/ClassName.h`):
```cpp
#pragma once

namespace orogena {

class ClassName {
public:
    ClassName();
    ~ClassName();

private:
    int m_memberVariable;
};

} // namespace orogena
```

2. **Create implementation** (`src/component/ClassName.cpp`):
```cpp
#include "component/ClassName.h"

namespace orogena {

ClassName::ClassName()
    : m_memberVariable(0)
{
}

ClassName::~ClassName() = default;

} // namespace orogena
```

3. **Add to CMakeLists.txt**:
```cmake
add_library(orogena_component
    component/ClassName.cpp
    # ... other files
)
```

4. **Write tests**:
```cpp
#include <gtest/gtest.h>
#include "component/ClassName.h"

TEST(ClassNameTest, BasicFunctionality) {
    orogena::ClassName instance;
    // Test code
}
```

### Building and Testing

```bash
# Rebuild
cd build
cmake --build . -j$(nproc)

# Run tests
ctest --output-on-failure

# Run specific test
./tests/test_unit --gtest_filter=ClassNameTest.*

# Run application
./src/orogena
```

---

## Common Tasks

### Adding a New Component

```bash
# 1. Create files
touch src/category/NewComponent.{h,cpp}

# 2. Write code (follow coding standard)

# 3. Add to CMakeLists.txt in src/CMakeLists.txt

# 4. Write tests
touch tests/unit/test_new_component.cpp

# 5. Add test to tests/CMakeLists.txt

# 6. Build and test
cd build && cmake --build . && ctest
```

### Debugging

**Linux (GDB)**:
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
gdb ./src/orogena
```

**macOS (LLDB)**:
```bash
lldb ./build/src/orogena
```

**Windows (Visual Studio)**:
- Open solution in Visual Studio
- Set orogena as startup project
- Press F5 to debug

### Profiling Performance

**Linux (perf)**:
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
perf record ./src/orogena
perf report
```

---

## Coding Standards Quick Reference

### Naming
- **Classes**: `PascalCase`
- **Methods**: `camelCase()`
- **Variables**: `camelCase`
- **Members**: `m_camelCase`
- **Constants**: `kPascalCase`

### Style
- **Braces**: Allman (on new line)
- **Indent**: 4 spaces
- **Line length**: 100 characters
- **Pointers**: `int* ptr` (attach to type)

### Best Practices
- Use `auto` for obvious types
- Prefer `const` by default
- Use smart pointers over raw pointers
- Range-based for loops when possible
- Mark methods `const` when they don't modify state

---

## IDE Setup

### Qt Creator (Recommended)

1. Open CMakeLists.txt as project
2. Configure with vcpkg toolchain
3. Set build directory to `build/`
4. Enable clang-format integration

### Visual Studio Code

1. Install C++ extension
2. Install CMake Tools extension
3. Configure CMake kit to use vcpkg toolchain
4. Install clang-format extension

### Visual Studio (Windows)

1. File → Open → CMake...
2. Select root CMakeLists.txt
3. Configure CMake settings to use vcpkg

---

## Troubleshooting

### Build Errors

**"Qt6 not found"**:
```bash
# Set Qt path
export CMAKE_PREFIX_PATH=/path/to/qt6
```

**"vcpkg dependencies fail"**:
```bash
# Update vcpkg
cd $VCPKG_ROOT
git pull
./vcpkg update
```

**"OpenGL headers not found"**:
```bash
# Linux: Install development packages
sudo pacman -S mesa opengl-driver  # Arch
sudo apt install libgl-dev          # Ubuntu
```

### Runtime Errors

**"No database driver"**:
- Qt SQL SQLite driver not installed
- Rebuild Qt with SQLite support

**"OpenGL context creation failed"**:
- Update graphics drivers
- Check OpenGL version: `glxinfo | grep "OpenGL version"`

---

## Getting Help

1. **Documentation**: Check `docs/` folder
2. **Code examples**: See existing implementations
3. **Standards**: Refer to coding and design standards
4. **Git history**: Look at previous commits for patterns

---

## Phase 1 Checklist

Current phase (Weeks 1-2):

- [x] Project structure
- [x] Build system
- [x] Logging
- [x] Basic UI
- [ ] Database wrapper
- [ ] OpenGL viewport
- [ ] Settings system
- [ ] Project save/load

---

## Next Steps After Setup

1. Read the [Software Development Plan](docs/001_sdp_orogena.md)
2. Complete remaining Phase 1 tasks
3. Implement Phase 2: Global Grid & Rendering
4. Follow the 9-phase roadmap

**Happy coding!**
