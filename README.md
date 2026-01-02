<p align="center">
    <picture>
        <source srcset="docs/assets/orogena_logo_dark.png" media="(prefers-color-scheme: dark)">
        <source srcset="docs/assets/orogena_logo.png" media="(prefers-color-scheme: light)">
        <img src="docs/assets/orogena_logo.png" alt="Orogena Logo" width="200">
    </picture>
</p>

<p align="center">Complete Worldbuilding Suite - Version 2.0</p>

<p align="center">
  <a href="https://github.com/tdiego1/orogena/actions/workflows/ci.yml">
    <img src="https://github.com/tdiego1/orogena/workflows/CI/badge.svg" alt="CI Status">
  </a>
  <a href="LICENSE">
    <img src="https://img.shields.io/badge/license-GPL--3.0%20%7C%20Commercial-blue.svg" alt="License">
  </a>
  <a href="https://github.com/tdiego1/orogena/releases">
    <img src="https://img.shields.io/github/v/release/tdiego1/orogena?include_prereleases" alt="Release">
  </a>
</p>

## Overview

Orogena is a professional-grade worldbuilding application following [Artifexian's methodology](https://www.youtube.com/c/Artifexian), enabling the creation of geologically and climatologically plausible worlds from stellar parameters down to local detail.

### Key Features

- **Complete 19-system worldbuilding**: Stars → Planets → Tectonics → Climate → Resources
- **Multi-scale generation**: Planet → Continent → Region → Local detail
- **GPU-accelerated** simulation for tectonics, climate, and ocean currents
- **Realistic erosion** with drainage networks and river systems
- **Köppen climate zones** with biome mapping
- **Resource placement**: Fuel, ores, and salt deposits in geologically appropriate locations
- **Seamless tile boundaries** using intelligent border stitching
- **Database-backed caching** for instant revisits
- **Cross-platform** support (Linux, Windows, macOS)

### The 19 Worldbuilding Systems

| Category | Systems |
|----------|---------|
| **Foundation** | Star & planetary system, planet parameters, moons & tides |
| **Geophysics** | Plate tectonics, land topography, bathymetry |
| **Circulation** | Ocean currents, winds & pressure, upwelling & reefs |
| **Climate** | Precipitation, temperature, Köppen zones, biomes |
| **Hydrology** | Rivers, lakes, drainage networks, weather patterns |
| **Resources** | Rocks & minerals, fuel deposits, copper/bronze/iron ores, salt |

---

## Building from Source

### Prerequisites

- **Compiler**: Clang 21+ (recommended), GCC 10+, or MSVC 2019+
- **CMake**: 3.25+ (for CMake Presets support)
- **vcpkg**: For dependency management
- **Qt**: 6.8+
- **Ninja**: Build system (optional but recommended)

### Quick Start (Using CMake Presets)

```bash
# Clone the repository
git clone https://github.com/tdiego1/orogena.git
cd orogena

# Setup vcpkg (first time only)
./scripts/setup_vcpkg.sh
source ~/.bashrc  # Reload shell

# One-command build (configure + build + test)
cmake --workflow --preset debug

# Run
./build/debug/src/orogena
```

**First time setup?** See [SETUP.md](SETUP.md) for detailed setup instructions.

**Build documentation:** See [BUILD_GUIDE.md](BUILD_GUIDE.md) for comprehensive build options.

### Platform-Specific Instructions

**Linux (Arch/CachyOS)**:
```bash
sudo pacman -S cmake clang ninja qt6-base mesa
cmake --workflow --preset release
```

**Windows** (using Visual Studio 2022):
```powershell
cmake --preset release
cmake --build --preset release
```

**macOS**:
```bash
brew install cmake llvm ninja qt@6
cmake --workflow --preset release
```

### Development vs Production Builds

```bash
# Development build (fast compile, debug symbols, tests)
cmake --workflow --preset dev

# Production build (optimized, LTO enabled)
cmake --workflow --preset release
```

### Available Build Presets

| Preset | Use Case | Tests | LTO | Speed |
|--------|----------|-------|-----|-------|
| `debug` / `dev` | Development | ✓ | ✗ | Fast compile |
| `release` | Production | ✗ | ✓ | Slow compile, fast runtime |
| `relwithdebinfo` | Profiling | ✓ | ✗ | Balanced |

See [PRESETS_SUMMARY.md](PRESETS_SUMMARY.md) for quick reference.

---

## Project Structure

```
orogena/
├── docs/               # Documentation
├── src/                # Source code
│   ├── core/          # Application foundation
│   ├── stellar/       # Stars, orbits, habitable zones
│   ├── planetary/     # Planet params, moons, tides
│   ├── global/        # Plate tectonics simulation
│   ├── topography/    # Land & bathymetry generation
│   ├── ocean/         # Ocean currents, upwelling
│   ├── atmosphere/    # Winds, pressure systems
│   ├── climate/       # Temperature, precipitation, Köppen
│   ├── hydrology/     # Rivers, lakes, drainage
│   ├── weather/       # Weather patterns
│   ├── geology/       # Rocks, minerals, cratons
│   ├── resources/     # Fuel, ores, salt deposits
│   ├── region/        # Regional tile system
│   ├── local/         # Local detail synthesis
│   ├── render/        # OpenGL visualization
│   ├── gui/           # Qt user interface
│   ├── database/      # SQLite persistence
│   └── utils/         # Logging and utilities
├── tests/             # Unit and integration tests
├── resources/         # Icons, UI files, presets
└── scripts/           # Build and packaging scripts
```

---

## Documentation

### Getting Started
- **[SETUP.md](SETUP.md)** - First-time setup guide (start here!)
- **[GETTING_STARTED.md](GETTING_STARTED.md)** - Basic usage tutorial
- **[BUILD_GUIDE.md](BUILD_GUIDE.md)** - Comprehensive CMake Presets guide

### Development
- [Software Development Plan v2.0](docs/001_sdp_orogena_v2.md) - Complete 12-phase roadmap
- [Coding Standard](docs/002_coding_standard.md) - Code style guide
- [Design Standard](docs/003_design_standard.md) - Architecture patterns

---

## Development Status

**Current Phase**: Phase 0 - Foundation Completion

### Roadmap

| Phase | Milestone | Status |
|-------|-----------|--------|
| **0** | Foundation (database, settings, viewport) | In Progress |
| **1** | Stellar Foundation | Planned |
| **2** | Planetary Parameters | Planned |
| **3** | Enhanced Plate Tectonics | Planned |
| **4** | Topography Generation | **MVP v1.0** |
| **5-11** | Ocean → Climate → Resources | Planned |
| **12** | Integration & Polish | **Full Suite v2.0** |

**MVP (v1.0)**: Complete terrain generation from stellar parameters
**Full Suite (v2.0)**: All 19 Artifexian worldbuilding systems

See the [Software Development Plan v2.0](docs/001_sdp_orogena_v2.md) for detailed timeline.

---

## Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](.github/CONTRIBUTING.md) for our development workflow and the [Coding Standard](docs/002_coding_standard.md) before submitting pull requests.

All pull requests must:
- Pass CI checks on Linux, Windows, and macOS
- Follow the established code style (enforced by `clang-format`)
- Include tests for new functionality
- Build without warnings (`-Werror` in CI)

---

## License

Orogena is dual-licensed:

### For Personal & Open Source Use

This project is free software licensed under the **GNU Public License v3.0 (GPL-3.0)**. You can freely use, modify, and distribute this software for personal, educational, and open-source projects.
See [LICENSE](LICENSE) for the full GPL-3.0 text.

### For Commercial Use

If you want to use Orogena in a commercial, closed-source project (such as a commercial game or proprietary application) without complying with GPL-3.0 requirements, you must obtain a **commercial license**.

**Contact** tdiego001@gmail.com for commercial licensing inquiries.

### Third-Party Licenses

Orogena uses the following open-source libraries:
- **Qt 6** - LGPL v3 (dynamically linked)
- **GLM** - MIT License
- **spdlog** - MIT License
- **nlohmann-json** - MIT License
- **libassert** - MIT License
- **SQLite** - Public Domain
- **GoogleTest** - BSD 3-Clause License

---

## Acknowledgments

- Worldbuilding methodology inspired by [Artifexian](https://www.youtube.com/c/Artifexian)
- Built with Qt 6, OpenGL, and modern C++20
- Geological accuracy informed by plate tectonics and climate science research
