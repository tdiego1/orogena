# Orogena Coding Standard

**Version**: 1.0
**Date**: December 2025
**Language**: C++20

## Key Conventions Summary

- **Files**: `lower_snake_case` with component prefix (e.g., `global_plate_manager.h`)
- **Namespaces**: Two-level PascalCase (e.g., `Orogena::Global`)
- **Classes/Methods**: PascalCase (e.g., `CalculateUplift()`)
- **Parameters**: camelCase (e.g., `plateCount`)
- **Local variables**: lower_snake_case (e.g., `active_plates`)
- **Members**: `m_PascalCase` (e.g., `m_Plates`)
- **Struct Members**: `pascalCase` (e.g., `plateType`)
- **Statics**: `s_PascalCase` (e.g., `s_Instance`)
- **Globals**: `g_PascalCase` (e.g., `g_Handle`)
- **Constants**: `c_PascalCase` (e.g., `c_MaxPlates`)
- **Enum values**: UPPER_SNAKE_CASE (e.g., `PlateType::OCEANIC`)
- **Strict Types**: Use `int32_t`/`float32_t` etc. from `utils/utils_types.h` (not `int`/`float`)

---

## 1. General Principles

### 1.1 Core Values
- **Readability**: Code is read far more often than written
- **Consistency**: Follow established patterns throughout the codebase
- **Simplicity**: Prefer simple solutions over clever ones
- **Performance**: Optimize critical paths, but profile first
- **Safety**: Prefer compile-time checks and RAII over manual management

### 1.2 Code Philosophy
- Favor composition over inheritance
- Use modern C++20 features appropriately
- Avoid premature optimization
- Write self-documenting code; use comments to explain "why", not "what"
- Design for testability

---

## 2. File Organization

### 2.1 File Naming
- **Headers**: `<component>_<class>.h` (lower_snake_case)
- **Implementation**: `<component>_<class>.cpp` (lower_snake_case)
- **Shaders**: `descriptive_name.{vert,frag,comp}` (lower_snake_case)
- **Tests**: `test_feature_name.cpp` (lower_snake_case)

**Examples**:
```
global_tectonic_simulation.h
global_tectonic_simulation.cpp
rendering_terrain_vertex.vert
test_plate_physics.cpp
```

**Rationale**: Lower snake case file names avoid case-sensitivity issues across different filesystems (Windows is case-insensitive, Linux/macOS are case-sensitive). The component prefix helps organize files when viewing flat directory listings.

### 2.2 File Structure

**Note**: Complete templates are available in [docs/templates/](templates/) directory:
- `header_template.h` / `source_template.cpp` - Professional templates for all classes
- See [TEMPLATE_GUIDE.md](templates/TEMPLATE_GUIDE.md) for usage instructions
- **We use `.h` extension** (not `.hpp`) following modern C++ convention

**Header file template** (simplified example):
```cpp
#pragma once

// System includes (alphabetically sorted)
#include <memory>
#include <vector>

// Qt includes (alphabetically sorted)
#include <QString>
#include <QVector3D>

// Third-party includes (alphabetically sorted)
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

// Project includes (alphabetically sorted)
#include "global/global_plate.h"
#include "utils/utils_logger.h"

namespace Orogena {
namespace Core {

/**
 * @brief Brief one-line description of the class
 *
 * Detailed description explaining the purpose, responsibilities,
 * and key design decisions. Include usage examples if helpful.
 */
class ClassName {
public:
    // Public Types
    enum class Status { IDLE, RUNNING };

    // Public Functions
    ClassName();
    explicit ClassName(int parameter);
    ~ClassName();

    // Rule of Five (delete/default as appropriate)
    ClassName(const ClassName&) = delete;
    ClassName& operator=(const ClassName&) = delete;
    ClassName(ClassName&&) = default;
    ClassName& operator=(ClassName&&) = default;

    void DoSomething();

    // Public Members (avoid - prefer getters/setters)

protected:
    // Protected Types
    // Protected Functions
    // Protected Members

private:
    // Private Types
    struct InternalData { };

    // Private Functions
    void HelperMethod();

    // Private Members
    Status m_Status;
};

} // namespace Core
} // namespace Orogena
```

**Implementation file template**:
```cpp
#include "core_class_name.h"

// Additional includes needed only for implementation
#include <algorithm>

namespace Orogena {
namespace Core {

ClassName::ClassName()
    : m_MemberVariable(0)
    , m_AnotherMember(nullptr)
{
    // Constructor body
}

// Method implementations...

} // namespace Core
} // namespace Orogena
```

### 2.3 Include Guards
- **Use**: `#pragma once` (supported by all modern compilers)
- **Avoid**: Traditional header guards (too verbose)

---

## 3. Naming Conventions

### 3.1 General Rules
- Use descriptive, self-documenting names
- Avoid abbreviations unless universally understood (e.g., `min`, `max`, `id`)
- Use American English spelling

### 3.2 Specific Conventions

| Element | Convention | Example |
|---------|-----------|---------|
| Namespace (top-level) | PascalCase | `Orogena` |
| Namespace (sub) | PascalCase | `Orogena::Core`, `Orogena::Rendering` |
| Class/Struct | PascalCase | `TectonicSimulation`, `HeightMap` |
| Function/Method | PascalCase | `CalculateUplift()`, `GetPlateCount()` |
| Parameter/Argument | camelCase | `plateVelocity`, `regionIndex` |
| Variable (local) | lower_snake_case | `plate_velocity`, `region_index` |
| Member variable | m_ + PascalCase | `m_Plates`, `m_HeightMap` |
| Struct variable | pascalCase | `plateType`, `boundaryStrength` |
| Static variable | s_ + PascalCase | `s_Instance`, `s_GlobalState` |
| Global variable | g_ + PascalCase | `g_ApplicationHandle` |
| Constant (any scope) | c_ + PascalCase | `c_MaxPlates`, `c_DefaultSeed` |
| Enum class | PascalCase | `PlateType`, `BoundaryType` |
| Enum value | UPPER_SNAKE_CASE | `PlateType::OCEANIC`, `BoundaryType::CONVERGENT` |
| Template param | PascalCase | `template<typename T>` |
| Macro | UPPER_SNAKE_CASE | `OROGENA_VERSION` |

### 3.3 Strict Type Definitions

Orogena uses explicit-width type aliases defined in `utils/utils_types.h` to ensure consistent type sizes across platforms and improve code clarity. **Always use these strict types instead of primitive types.**

```cpp
#include "utils/utils_types.h"
```

| Primitive Type | Strict Type | Purpose |
|---------------|-------------|---------|
| `char` | `char_t` | Character data |
| `int8_t` / `std::int8_t` | `int8_t` | 8-bit signed integer |
| `int16_t` / `std::int16_t` | `int16_t` | 16-bit signed integer |
| `int` / `int32_t` | `int32_t` | 32-bit signed integer |
| `long long` / `int64_t` | `int64_t` | 64-bit signed integer |
| `uint8_t` / `std::uint8_t` | `uint8_t` | 8-bit unsigned integer |
| `uint16_t` / `std::uint16_t` | `uint16_t` | 16-bit unsigned integer |
| `unsigned int` / `uint32_t` | `uint32_t` | 32-bit unsigned integer |
| `unsigned long long` / `uint64_t` | `uint64_t` | 64-bit unsigned integer |
| `float` | `float32_t` | 32-bit floating point |
| `double` | `float64_t` | 64-bit floating point |

**Examples**:
```cpp
// Bad - ambiguous primitive types
int plate_count = 10;
float height = 1500.0f;
char buffer[256];

// Good - explicit strict types
int32_t plate_count = 10;
float32_t height = 1500.0f;
char_t buffer[256];

// Function signatures should use strict types
float64_t CalculateUplift(int32_t plateIndex, float64_t deltaTime);
void ProcessHeightMap(const std::vector<float32_t>& heights);

// Member variables with strict types
class HeightMap
{
private:
    int32_t m_Width;
    int32_t m_Height;
    std::vector<float32_t> m_Data;
};
```

**When to use which integer size**:
- `int32_t`: Default choice for counts, indices, and general integers
- `int64_t`: Large values (file sizes, timestamps, IDs that may exceed 2 billion)
- `int8_t`/`int16_t`: Memory-constrained data structures, binary protocols
- Unsigned variants: Bit manipulation, sizes, truly non-negative values

**When to use which floating-point size**:
- `float32_t`: Graphics data, GPU buffers, memory-sensitive arrays (heightmaps)
- `float64_t`: Simulation calculations, accumulation, precision-sensitive math

**Exceptions**:
- Loop indices with `auto` (e.g., range-based for loops) are acceptable
- Third-party API boundaries may require their native types
- `size_t` for STL container sizes and indices remains acceptable
- `bool` does not have a strict type alias (use `bool` directly)

**Prefix Priority Rules**:
- If a variable is both static and global, use `s_` prefix
- If a variable is constant (and static/global), use `c_` prefix (constant takes precedence)
- Member variables always use `m_` prefix
- The prefix letter(s) count as the first character (e.g., `m_Plates` not `m_plates`)

**Examples**:
```cpp
namespace Orogena {
namespace Global {

// Constants use c_ prefix
constexpr int c_MaxPlateCount = 50;
constexpr double c_EarthRadius = 6371.0; // km

// Static variables use s_ prefix
static int s_GlobalPlateCounter = 0;

// Global variable would use g_ prefix (avoid when possible)
// int g_ApplicationHandle;

enum class PlateType {
    OCEANIC,
    CONTINENTAL,
    TRANSITIONAL
};

class PlateManager {
public:
    // Methods are PascalCase
    void UpdatePlates(double deltaTime);
    int GetPlateCount() const { return m_Plates.size(); }

private:
    // Private methods are also PascalCase
    void CalculateForces();

    // Member variables use m_ + PascalCase
    std::vector<Plate> m_Plates;
    double m_SimulationTime;

    // Static member uses s_ + PascalCase
    static PlateManager* s_Instance;
};

void ExampleFunction(int plateCount, double timeStep)
{
    // Parameters are camelCase
    // Local variables are lower_snake_case
    int active_plates = plateCount;
    double current_time = 0.0;

    for (int i = 0; i < active_plates; ++i)
    {
        current_time += timeStep;
    }
}

} // namespace Global
} // namespace Orogena
```

---

## 4. Formatting

### 4.1 Indentation and Spacing
- **Indentation**: 4 spaces (no tabs)
- **Line length**: 100 characters maximum (soft limit)
- **Braces**: Allman style (braces on new line)

```cpp
if (condition)
{
    DoSomething();
}
else
{
    DoSomethingElse();
}

for (const auto& plate : m_Plates)
{
    plate.Update();
}

class MyClass
{
public:
    void method()
    {
        // Implementation
    }
};
```

### 4.2 Spacing Rules
```cpp
// Binary operators: spaces around
int result = a + b * c;
bool valid = (x > 0) && (y < 100);

// Unary operators: no space
++counter;
*pointer;
!flag;

// Function calls: no space before parenthesis
doSomething(arg1, arg2);

// Control flow: space before parenthesis
if (condition)
while (running)
for (int i = 0; i < count; ++i)

// Pointers and references: attach to type
int* pointer;
const std::string& reference;
void function(Plate* plate, const Region& region);
```

### 4.3 Initialization
Prefer uniform initialization with braces:
```cpp
// Preferred
int value{42};
std::vector<int> numbers{1, 2, 3, 4, 5};
Plate plate{PlateType::Oceanic, velocity};

// Constructor initialization lists: one per line for readability
ClassName::ClassName(int a, double b, const std::string& c)
    : m_valueA(a)
    , m_valueB(b)
    , m_valueC(c)
{
}
```

---

## 5. Language Features

### 5.1 Modern C++20 Usage

**Use `auto` appropriately**:
```cpp
// Good: obvious types, iterators
auto position = glm::vec3{0.0f, 0.0f, 0.0f};
auto it = m_plates.begin();

// Bad: unclear types
auto x = calculate(); // What type is x?

// Better: explicit when not obvious
double uplift = calculateUplift();
```

**Range-based for loops**:
```cpp
// Prefer range-based loops
for (const auto& plate : m_plates)
{
    processPlate(plate);
}

// Use structured bindings
for (const auto& [key, value] : m_regionCache)
{
    updateRegion(key, value);
}
```

**Smart pointers**:
```cpp
// Prefer smart pointers over raw pointers
std::unique_ptr<Simulation> m_simulation;
std::shared_ptr<Database> m_database;

// Use std::make_unique and std::make_shared
auto sim = std::make_unique<TectonicSimulation>();
auto db = std::make_shared<DatabaseManager>();
```

**Concepts** (when appropriate):
```cpp
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

template<Numeric T>
T clamp(T value, T min, T max)
{
    return std::max(min, std::min(max, value));
}
```

**Span for array views**:
```cpp
void processHeightData(std::span<const float> heights)
{
    for (float height : heights)
    {
        // Process height
    }
}
```

### 5.2 Const Correctness
- Mark all variables `const` when they won't change
- Mark all methods `const` when they don't modify state
- Use `const&` for parameters (unless trivial types or need to copy)

```cpp
class HeightMap
{
public:
    // Const method - doesn't modify state
    float getHeight(int x, int y) const;

    // Non-const method - modifies state
    void setHeight(int x, int y, float value);

    // Const reference parameter
    void copyFrom(const HeightMap& other);

private:
    std::vector<float> m_data;
};
```

### 5.3 Avoid
- **Raw `new`/`delete`**: Use smart pointers or containers
- **C-style casts**: Use `static_cast`, `dynamic_cast`, `const_cast`, `reinterpret_cast`
- **Raw arrays**: Use `std::array` or `std::vector`
- **Manual memory management**: Use RAII
- **`NULL`**: Use `nullptr`
- **`typedef`**: Use `using` alias

```cpp
// Bad
int* array = new int[100];
delete[] array;
void* ptr = (void*)object;

// Good
std::vector<int> array(100);
auto data = std::make_unique<int[]>(100);
void* ptr = static_cast<void*>(object);
```

---

## 6. Comments and Documentation

### 6.1 Comment Style
- Use `//` for single-line comments
- Use `/** */` for documentation comments (Doxygen style)
- Write comments that explain "why", not "what"

### 6.2 Documentation Comments
```cpp
/**
 * @brief Calculates uplift rate at plate boundaries
 *
 * Uses simplified collision physics to determine mountain-building
 * rates based on plate velocity and convergence angle. The algorithm
 * assumes rigid plates and does not account for plate deformation.
 *
 * @param boundary The boundary between two plates
 * @param deltaTime Time step in millions of years
 * @return Uplift rate in meters per million years
 *
 * @note This is a simplified model for performance reasons
 * @see GeologyEngine::calculateOrogeny() for full deformation model
 */
double calculateUpliftRate(const PlateBoundary& boundary, double deltaTime);
```

### 6.3 Inline Comments
```cpp
// Good: explains why
// Use overlap to ensure seamless stitching between tiles
constexpr int kBorderOverlap = 10; // km

// Bad: states the obvious
// Set i to 0
int i = 0;
```

### 6.4 TODO Comments
```cpp
// TODO(username): Brief description of what needs to be done
// FIXME(username): Description of known bug
// HACK(username): Explanation of why this workaround exists
// NOTE(username): Important information for future maintainers
```

---

## 7. Error Handling

### 7.1 Exceptions
- Use exceptions for exceptional conditions (not control flow)
- Provide meaningful exception messages
- Document exceptions in function documentation

```cpp
/**
 * @throws std::invalid_argument if plateCount is outside valid range
 * @throws std::runtime_error if simulation initialization fails
 */
void initializeSimulation(int plateCount)
{
    if (plateCount < kMinPlates || plateCount > kMaxPlates)
    {
        throw std::invalid_argument(
            std::format("Plate count {} outside valid range [{}, {}]",
                       plateCount, kMinPlates, kMaxPlates)
        );
    }

    // Initialization code...
}
```

### 7.2 Assertions
- Use `assert()` for debugging/development checks
- Use Qt's `Q_ASSERT()` in Qt-dependent code
- Assertions should never have side effects

```cpp
#include <cassert>

void updatePlate(Plate* plate)
{
    assert(plate != nullptr && "Plate pointer must not be null");

    // Update logic...
}
```

### 7.3 Logging
Use spdlog for logging with appropriate levels:

```cpp
#include "utils/Logger.h"

void performSimulation()
{
    spdlog::info("Starting tectonic simulation with {} plates", plateCount);
    spdlog::debug("Simulation parameters: seed={}, time={}", seed, timeSpan);

    if (error)
    {
        spdlog::error("Simulation failed: {}", errorMessage);
    }

    spdlog::warn("Performance degraded: using CPU fallback");
}
```

**Log levels**:
- `trace`: Very detailed information
- `debug`: Debugging information
- `info`: General informational messages
- `warn`: Warning messages
- `error`: Error conditions
- `critical`: Critical failures

---

## 8. Performance Guidelines

### 8.1 General Rules
- Profile before optimizing
- Optimize critical paths only (simulation, rendering)
- Document performance-critical sections

### 8.2 Hot Path Optimization
```cpp
// Mark hot paths for compiler optimization
[[likely]] / [[unlikely]] attributes

if (isCommonCase) [[likely]]
{
    // Fast path
}
else [[unlikely]]
{
    // Rare case
}

// Use constexpr for compile-time constants
constexpr double calculateConstant()
{
    return 3.14159 * 2.0;
}
```

### 8.3 Memory Efficiency
```cpp
// Reserve capacity for vectors when size is known
std::vector<Plate> plates;
plates.reserve(plateCount);

// Use emplace instead of push for in-place construction
plates.emplace_back(PlateType::Oceanic, velocity);

// Pass large objects by const reference
void processRegion(const HeightMap& heightMap);
```

### 8.4 Avoid in Hot Loops
- Memory allocation
- Logging (except at error level)
- String operations
- Exception throwing

---

## 9. Qt-Specific Guidelines

### 9.1 Signals and Slots
```cpp
class SimulationController : public QObject
{
    Q_OBJECT

public:
    explicit SimulationController(QObject* parent = nullptr);

signals:
    void simulationProgress(int percentage);
    void simulationCompleted();

public slots:
    void startSimulation();
    void pauseSimulation();

private slots:
    void onSimulationStep();
};
```

### 9.2 Qt Naming
- Follow Qt conventions for Qt-specific code
- Use Qt containers (`QVector`, `QString`) at Qt API boundaries
- Use STL containers internally for better portability

### 9.3 Parent-Child Ownership
```cpp
// Qt objects with parent are automatically deleted
auto* widget = new CustomWidget(this); // 'this' is the parent

// No need for explicit delete - Qt handles it
```

---

## 10. Testing Guidelines

### 10.1 Test Naming
```cpp
// Test file: test_plate_physics.cpp
TEST(PlatePhysicsTest, CalculatesCorrectUpliftAtConvergentBoundary)
{
    // Arrange
    Plate oceanic{PlateType::Oceanic};
    Plate continental{PlateType::Continental};

    // Act
    double uplift = calculateUplift(oceanic, continental);

    // Assert
    EXPECT_GT(uplift, 0.0);
}
```

### 10.2 Test Structure
- Use Arrange-Act-Assert pattern
- One logical assertion per test
- Test edge cases and error conditions
- Use meaningful test names that describe the scenario

---

## 11. CMake Guidelines

### 11.1 Target Definition
```cmake
add_library(orogena_core
    src/core/Application.cpp
    src/core/Project.cpp
    src/core/Settings.cpp
)

target_include_directories(orogena_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(orogena_core
    PUBLIC
        Qt6::Core
        Qt6::Widgets
        glm::glm
)
```

### 11.2 Naming
- Libraries: `orogena_<component>`
- Executables: `orogena` or `orogena_<tool>`
- Tests: `test_<feature>`

---

## 12. Version Control

### 12.1 Commit Messages
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

### 12.2 Branch Naming
- `main`: Stable release branch
- `develop`: Integration branch
- `feature/<name>`: Feature branches
- `fix/<name>`: Bug fix branches
- `refactor/<name>`: Refactoring branches

---

## 13. Code Review Checklist

Before submitting code for review (or self-review):

- [ ] Follows naming conventions
- [ ] Properly formatted (4 spaces, Allman braces)
- [ ] Const-correct
- [ ] No memory leaks (RAII, smart pointers)
- [ ] Documented (public API has doc comments)
- [ ] Tested (unit tests for new functionality)
- [ ] No compiler warnings
- [ ] Follows single responsibility principle
- [ ] Error conditions handled appropriately
- [ ] Performance considered for hot paths

---

## 14. Tools and Automation

### 14.1 Recommended Tools
- **Formatter**: clang-format (config provided in `.clang-format`)
- **Linter**: clang-tidy (config in `.clang-tidy`)
- **Static analysis**: cppcheck
- **Profiler**: perf (Linux), Instruments (macOS), Visual Studio Profiler (Windows)

### 14.2 Pre-commit Hooks
Set up Git hooks to run formatting and basic checks before commit.

---

## 15. Platform-Specific Notes

### 15.1 Cross-Platform Code
```cpp
// Use Qt's cross-platform facilities
QString filePath = QStandardPaths::writableLocation(
    QStandardPaths::AppDataLocation
);

// Avoid platform-specific code when possible
// If needed, use preprocessor guards
#ifdef Q_OS_WIN
    // Windows-specific code
#elif defined(Q_OS_MACOS)
    // macOS-specific code
#elif defined(Q_OS_LINUX)
    // Linux-specific code
#endif
```

---

## Appendix A: Example .clang-format

```yaml
BasedOnStyle: LLVM
IndentWidth: 4
BreakBeforeBraces: Allman
ColumnLimit: 100
PointerAlignment: Left
AllowShortFunctionsOnASingleLine: Empty
AllowShortIfStatementsOnASingleLine: Never
IndentCaseLabels: true
NamespaceIndentation: None
```

---

## Appendix B: Quick Reference

```cpp
// File names (lower_snake_case with component prefix)
global_tectonic_simulation.h
global_tectonic_simulation.cpp

// Namespaces (PascalCase, two levels)
namespace Orogena {
namespace Global {
    // ...
} // namespace Global
} // namespace Orogena

// Class (PascalCase)
class TectonicSimulation { };

// Methods/Functions (PascalCase)
void CalculateForces();
int GetPlateCount() const;

// Parameters (camelCase)
void UpdatePlate(int plateIndex, double deltaTime);

// Local variables (lower_snake_case)
int active_plate_count = 10;
double simulation_time = 0.0;

// Member variables (m_ + PascalCase)
std::vector<Plate> m_Plates;
double m_SimulationTime;

// Static variables (s_ + PascalCase)
static int s_InstanceCount;

// Global variables (g_ + PascalCase) - avoid when possible
int g_ApplicationHandle;

// Constants (c_ + PascalCase)
constexpr int c_MaxPlates = 50;
constexpr double c_EarthRadius = 6371.0;

// Enum class (PascalCase)
enum class PlateType { OCEANIC, CONTINENTAL };

// Enum values (UPPER_SNAKE_CASE)
PlateType::OCEANIC
PlateType::CONTINENTAL

// Smart pointers
std::unique_ptr<Simulation> m_Simulation;

// Range loop
for (const auto& plate : m_Plates) { }

// Lambda
auto callback = [this](int value) { return value * 2; };

// Prefix priority:
// - Constant always wins: c_MaxValue (even if static/global)
// - Static + global: s_SharedData
// - Member: m_Data
```

---

**Document Status**: Living document - will be updated as the project evolves
