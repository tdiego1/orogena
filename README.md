# Orogena

**Multi-Scale Tectonic Simulator for Fantasy World Building**

Version 1.0 | December 2025

---

## Overview

Orogena is a professional-grade terrain generation tool that simulates millions of years of plate tectonics at planetary scale, enabling seamless extraction of regional and local maps with progressive detail enhancement.

### Key Features

- **Multi-scale generation**: Planet → Continent → Region → Local detail
- **GPU-accelerated** global tectonic simulation
- **Realistic erosion** with drainage networks
- **Seamless tile boundaries** using intelligent border stitching
- **Database-backed caching** for instant revisits
- **Cross-platform** support (Linux, Windows, macOS)

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
git clone https://github.com/yourusername/orogena.git
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
│   ├── core/          # Application core
│   ├── global/        # Scale 1: Global simulation
│   ├── regional/      # Scale 2: Regional generation
│   ├── local/         # Scale 3: Local detail
│   ├── rendering/     # OpenGL rendering
│   ├── ui/            # Qt user interface
│   ├── database/      # SQLite database layer
│   └── utils/         # Utilities
├── tests/             # Unit and integration tests
├── resources/         # Icons, UI files, presets
└── scripts/           # Build and packaging scripts
```

---

## Documentation

### Getting Started
- **[SETUP.md](SETUP.md)** - First-time setup guide (start here!)
- **[BUILD_GUIDE.md](BUILD_GUIDE.md)** - Comprehensive CMake Presets guide
- **[PRESETS_SUMMARY.md](PRESETS_SUMMARY.md)** - Quick preset reference

### Development
- [Software Development Plan](docs/001_sdp_orogena.md) - Project roadmap
- [Coding Standard](docs/002_coding_standard.md) - Code style guide
- [Design Standard](docs/003_design_standard.md) - Architecture patterns
- [CLAUDE.md](CLAUDE.md) - AI-assisted development guide

---

## Development Status

**Current Phase**: Foundation & Infrastructure (Phase 1)

See the [Software Development Plan](docs/001_sdp_orogena.md) for detailed roadmap.

---

## Contributing

Contributions are welcome! Please read the coding and design standards before submitting pull requests.

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Follow the [Coding Standard](docs/002_coding_standard.md)
4. Write tests for new functionality
5. Commit your changes (`git commit -m 'feat: add amazing feature'`)
6. Push to the branch (`git push origin feature/amazing-feature`)
7. Open a Pull Request

---

## License

[License TBD]

---

## Acknowledgments

- Inspired by real-world plate tectonics and terrain generation research
- Built with Qt 6, OpenGL, and modern C++20
