# CI/CD Pipeline Documentation

This document describes the Continuous Integration and Continuous Deployment (CI/CD) pipeline for Orogena.

## Overview

Orogena uses **GitHub Actions** for automated building, testing, and releases across three platforms:
- Linux (Ubuntu with Clang 21)
- Windows (MSVC 2022)
- macOS (Homebrew LLVM)

## Workflows

### 1. CI Workflow (`.github/workflows/ci.yml`)

**Triggers:**
- Push to `main` or `develop` branches
- Pull requests targeting `main` or `develop`

**Jobs:**

#### Build & Test Jobs (Linux, Windows, macOS)

Each platform job performs:

1. **Checkout** - Clones repository with submodules
2. **Install Dependencies**
   - System packages (Clang, OpenGL, X11 libs on Linux)
   - Qt 6.8+ via `jurplel/install-qt-action`
   - vcpkg dependencies via `lukka/run-vcpkg`
3. **Configure** - Uses `ci` CMake preset
   - Build type: Release
   - Tests: Enabled
   - Warnings as errors: `-Werror`
4. **Build** - Parallel build with Ninja
5. **Test** - Runs all unit tests with `ctest`
6. **Upload Artifacts** - On failure, uploads CMake logs

**vcpkg Caching:**
The workflow uses GitHub Actions cache for vcpkg binary packages to speed up builds:
```yaml
env:
  VCPKG_BINARY_SOURCES: 'clear;x-gha,readwrite'
```

#### Code Quality Job

Runs on Linux only:

1. **Format Check** - Verifies all `.h` and `.cpp` files pass `clang-format-21`
2. **Whitespace Check** - Ensures no trailing whitespace in source files

**Failure Conditions:**
- Code doesn't match `.clang-format` style
- Trailing whitespace found
- Build fails on any platform
- Any test fails
- Compiler warnings present

### 2. Release Workflow (`.github/workflows/release.yml`)

**Triggers:**
- Push to tags matching `v*.*.*` (e.g., `v0.1.0`, `v1.0.0-beta`)

**Jobs:**

#### Create Release

1. Extracts version from git tag
2. Creates GitHub Release
3. Marks as prerelease if tag contains `alpha`, `beta`, or `rc`
4. Generates release notes automatically

#### Build Release Artifacts

Builds optimized binaries for all platforms:

1. Uses `release` CMake preset (LTO enabled, tests disabled)
2. Packages executable with LICENSE and README
3. Creates platform-specific archives:
   - Linux: `orogena-linux-x64.tar.gz`
   - Windows: `orogena-windows-x64.zip`
   - macOS: `orogena-macos-arm64.tar.gz`
4. Uploads archives to GitHub Release

## CMake Presets for CI

### `ci` Configure Preset

Defined in `CMakePresets.json`:

```json
{
    "name": "ci",
    "inherits": "default",
    "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release",
        "OROGENA_BUILD_TESTS": "ON",
        "CMAKE_CXX_FLAGS": "-Werror"
    }
}
```

**Key settings:**
- **Release mode** - Full optimizations without LTO (faster CI builds)
- **Tests enabled** - Runs full test suite
- **Warnings as errors** - Enforces code quality

### `ci` Workflow Preset

Runs configure → build → test in sequence:

```json
{
    "name": "ci",
    "steps": [
        { "type": "configure", "name": "ci" },
        { "type": "build", "name": "ci" },
        { "type": "test", "name": "ci" }
    ]
}
```

## Platform-Specific Details

### Linux (Ubuntu)

**Compiler:** Clang (latest available from Ubuntu repos) with libc++
**Dependencies:**
- `ninja-build` - Build system
- `clang`, `libc++-dev`, `libc++abi-dev` - Toolchain
- `libgl-dev`, `libglu1-mesa-dev` - OpenGL
- X11/XCB libraries for Qt

**Build commands:**
```bash
export CC=clang
export CXX=clang++
cmake --preset ci
cmake --build --preset ci -j$(nproc)
ctest --preset ci --output-on-failure
```

### Windows

**Compiler:** MSVC 2022 (latest)
**Dependencies:**
- Qt 6.8 (MSVC 2022 64-bit)
- vcpkg packages

**Notes:**
- Uses `ilammy/msvc-dev-cmd` to setup MSVC environment
- Requires `-C Release` flag for `ctest` (multi-config generator)

**Build commands:**
```powershell
cmake --preset ci
cmake --build --preset ci --config Release
ctest --preset ci --output-on-failure -C Release
```

### macOS

**Compiler:** Homebrew LLVM (Clang, latest)
**Dependencies:**
- Homebrew: `ninja`, `llvm`
- Qt 6.8 via `install-qt-action`

**Build commands:**
```bash
export CC=$(brew --prefix llvm)/bin/clang
export CXX=$(brew --prefix llvm)/bin/clang++
cmake --preset ci
cmake --build --preset ci -j$(sysctl -n hw.ncpu)
ctest --preset ci --output-on-failure
```

## Local CI Testing

Developers can test the exact CI configuration locally:

```bash
# Run full CI workflow (configure + build + test)
cmake --workflow --preset ci

# Step-by-step (useful for debugging)
cmake --preset ci
cmake --build --preset ci
ctest --preset ci --output-on-failure

# Check code formatting
find src tests -name "*.h" -o -name "*.cpp" | xargs clang-format --dry-run --Werror

# Check for trailing whitespace
git grep -I --line-number -e '[[:blank:]]$' -- '*.cpp' '*.h' '*.cmake' 'CMakeLists.txt'
```

## Troubleshooting CI Failures

### Build Failures

1. Check uploaded `CMakeFiles/CMakeOutput.log` and `CMakeError.log` artifacts
2. Reproduce locally with `cmake --workflow --preset ci`
3. Check compiler version matches CI environment

### Test Failures

1. Review test output in GitHub Actions logs
2. Run specific failing test locally:
   ```bash
   ./build/ci/tests/test_unit --gtest_filter=FailingTest.*
   ```
3. Check for platform-specific issues (e.g., path separators, OpenGL availability)

### Format Check Failures

1. Run locally:
   ```bash
   clang-format -i $(find src tests -name "*.cpp" -o -name "*.h")
   ```
2. Commit formatted files

### Dependency Issues

1. **vcpkg failures:**
   - Check `vcpkg` tag in workflow matches your local version
   - Clear GitHub Actions cache if vcpkg cache is corrupted
2. **Qt installation:**
   - Verify Qt version matches workflow (`6.8.*`)
   - Check for platform-specific Qt modules

## Best Practices

1. **Test locally before pushing:**
   ```bash
   cmake --workflow --preset ci && clang-format -i src/**/*.{h,cpp}
   ```

2. **Keep CI green:**
   - Don't push if local CI fails
   - Fix failing tests immediately
   - Don't disable warnings to make CI pass

3. **Optimize CI time:**
   - vcpkg dependencies are cached automatically
   - Qt installation is cached per version
   - Use parallel builds (`-j$(nproc)`)

4. **Release workflow:**
   - Tag format: `vMAJOR.MINOR.PATCH` (e.g., `v0.1.0`)
   - Prerelease: `v1.0.0-alpha.1`, `v1.0.0-beta.2`, `v1.0.0-rc.1`

## Status Badges

Add to `README.md`:

```markdown
[![CI Status](https://github.com/tdiego1/orogena/workflows/CI/badge.svg)](https://github.com/tdiego1/orogena/actions/workflows/ci.yml)
```

Current status: ![CI Status](https://github.com/tdiego1/orogena/workflows/CI/badge.svg)

## Future Enhancements

Planned CI improvements:

- [ ] Code coverage reporting (gcov/lcov)
- [ ] Static analysis (clang-tidy)
- [ ] Sanitizers (ASan, UBSan, TSan)
- [ ] Performance benchmarking
- [ ] Documentation generation (Doxygen)
- [ ] Nightly builds from `develop`

---

**Document Version:** 1.0
**Last Updated:** January 2026
**Related Files:**
- [.github/workflows/ci.yml](../.github/workflows/ci.yml)
- [.github/workflows/release.yml](../.github/workflows/release.yml)
- [CMakePresets.json](../CMakePresets.json)
- [CONTRIBUTING.md](../.github/CONTRIBUTING.md)
