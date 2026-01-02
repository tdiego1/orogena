# Contributing to Orogena

Thank you for your interest in contributing to Orogena!

## Development Workflow

1. Fork the repository
2. Create a feature branch from `develop`: `git checkout -b feature/your-feature develop`
3. Make your changes following the [Coding Standard](../docs/002_coding_standard.md)
4. Run tests locally: `cmake --workflow --preset debug`
5. Format your code: `clang-format -i src/**/*.{h,cpp}`
6. Commit your changes with a descriptive message
7. Push to your fork and submit a pull request to `develop`

## Code Standards

All code must:
- Follow the naming conventions in [CLAUDE.md](../CLAUDE.md)
- Pass `clang-format` formatting
- Build without warnings with the `ci` preset
- Include tests for new functionality
- Not break existing tests

The CI pipeline will automatically check these requirements.

## Pull Request Process

1. Ensure your PR description clearly describes the problem and solution
2. Reference any related issues
3. Wait for CI checks to pass (all three platforms: Linux, Windows, macOS)
4. Address any review feedback
5. Once approved, a maintainer will merge your PR

## CI Pipeline

Our GitHub Actions CI pipeline runs on every push and pull request:

### Build & Test Jobs
- **Linux**: Ubuntu with Clang 21, Qt 6.8+
- **Windows**: MSVC 2022, Qt 6.8+
- **macOS**: Latest with Clang (Homebrew LLVM)

Each platform:
1. Installs dependencies (Qt, vcpkg packages)
2. Configures with the `ci` preset (Release + tests + warnings as errors)
3. Builds the project
4. Runs all tests with `ctest`
5. Uploads logs on failure

### Code Quality Job
- Checks `clang-format` compliance
- Checks for trailing whitespace

## Local Testing

Before pushing, test locally with the CI preset:

```bash
# Full CI workflow (configure + build + test)
cmake --workflow --preset ci

# Or step by step
cmake --preset ci
cmake --build --preset ci
ctest --preset ci --output-on-failure
```

## Branching Strategy

- `main` - Stable releases only
- `develop` - Integration branch (target for PRs)
- `feature/*` - New features
- `fix/*` - Bug fixes
- `refactor/*` - Code refactoring

## Commit Message Format

```
<type>: <subject>

<body>

<footer>
```

**Types**: `feat`, `fix`, `docs`, `refactor`, `test`, `perf`, `chore`

**Example**:
```
feat: implement stream power erosion for regional tiles

- Add StreamPowerErosion class with configurable parameters
- Integrate erosion simulation into RegionManager
- Add unit tests for drainage network calculation

Closes #42
```

## Questions?

Feel free to open an issue for discussion before starting work on large features.
