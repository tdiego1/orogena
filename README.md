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

- C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
- CMake 3.20+
- vcpkg (for dependency management)
- Qt 6.8+

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/yourusername/orogena.git
cd orogena

# Configure with vcpkg
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake

# Build
cmake --build build --config Release

# Run
./build/src/orogena
```

### Linux (CachyOS/Arch)

```bash
# Install system dependencies
sudo pacman -S cmake gcc qt6-base qt6-3d opengl-driver

# Build with vcpkg
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build build -j$(nproc)
```

### Windows

```bash
# Using Visual Studio 2022
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

### macOS

```bash
# Install dependencies via Homebrew
brew install cmake qt@6

# Build
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

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

- [Software Development Plan](docs/001_sdp_orogena.md)
- [Coding Standard](docs/002_coding_standard.md)
- [Design Standard](docs/003_design_standard.md)

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
