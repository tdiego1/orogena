# Orogena Design Standard

**Version**: 1.0
**Date**: December 2025
**Purpose**: Architectural guidelines and design principles for Orogena development

---

## 1. Architecture Overview

### 1.1 System Architecture

Orogena follows a **layered, multi-scale architecture** organized around three simulation scales:

```
┌─────────────────────────────────────────────────────┐
│              UI Layer (Qt Widgets)                  │
├─────────────────────────────────────────────────────┤
│            Rendering Layer (OpenGL)                 │
├─────────────────────────────────────────────────────┤
│  ┌─────────┐  ┌──────────┐  ┌─────────────────┐     │
│  │ Scale 1 │  │ Scale 2  │  │    Scale 3      │     │
│  │ Global  │──│ Regional │──│ Local Detail    │     │
│  │  Sim    │  │   Gen    │  │  Synthesis      │     │
│  └─────────┘  └──────────┘  └─────────────────┘     │
├─────────────────────────────────────────────────────┤
│        Database Layer (SQLite + Cache)              │
├─────────────────────────────────────────────────────┤
│      Core Infrastructure (Logging, Config, IO)      │
└─────────────────────────────────────────────────────┘
```

### 1.2 Design Principles

1. **Separation of Concerns**: Each layer has a single, well-defined responsibility
2. **Modularity**: Components are loosely coupled and highly cohesive
3. **Testability**: Design for unit testing from the start
4. **Performance**: GPU-first approach for computationally intensive operations
5. **Scalability**: Handle planets from small (512²) to large (4096²) grids
6. **Maintainability**: Clear interfaces, comprehensive documentation

---

## 2. Layered Architecture

### 2.1 Layer Responsibilities

#### **Layer 1: UI Layer**
- **Responsibility**: User interaction, visualization controls, settings management
- **Technology**: Qt 6 Widgets
- **Components**: MainWindow, Viewports, Dialogs, Control Panels
- **Rules**:
  - No business logic in UI code
  - Communicate with lower layers via signals/slots
  - All UI operations on main thread

#### **Layer 2: Rendering Layer**
- **Responsibility**: Visual representation of simulation data
- **Technology**: OpenGL 4.5+ / Vulkan
- **Components**: Renderers, Camera, LOD Manager, Shader Pipeline
- **Rules**:
  - Read-only access to simulation data
  - All rendering on render thread
  - No simulation logic in rendering code

#### **Layer 3: Simulation Layer**
- **Responsibility**: Core geological simulation and terrain generation
- **Technology**: C++20, Compute Shaders
- **Components**: Scale 1/2/3 generators, Physics engines, Erosion simulators
- **Rules**:
  - Scale-specific modules communicate through well-defined interfaces
  - No direct UI dependencies
  - Thread-safe for parallel processing

#### **Layer 4: Database Layer**
- **Responsibility**: Persistent storage and caching
- **Technology**: SQLite
- **Components**: DatabaseManager, DAOs, Cache management
- **Rules**:
  - Abstract database access behind interfaces
  - Handle all serialization/deserialization
  - Connection pooling for thread safety

#### **Layer 5: Core Infrastructure**
- **Responsibility**: Cross-cutting concerns
- **Technology**: C++20, Qt Core, spdlog
- **Components**: Logger, Configuration, Threading, File I/O
- **Rules**:
  - No domain logic
  - Available to all layers
  - Platform-agnostic where possible

### 2.2 Layer Communication Rules

```
┌────────┐
│   UI   │ ← Can only call Rendering & Simulation APIs
└────────┘
     ↓
┌────────┐
│Render  │ ← Can only read from Simulation (via interfaces)
└────────┘
     ↓
┌────────┐
│  Sim   │ ← Can call Database & Core
└────────┘
     ↓
┌────────┐
│   DB   │ ← Can call Core only
└────────┘
     ↓
┌────────┐
│  Core  │ ← No dependencies on other layers
└────────┘
```

**Rules**:
- **Downward dependencies only**: Higher layers depend on lower layers, never the reverse
- **No layer skipping**: UI cannot directly access Database
- **Callbacks for upward communication**: Use signals/slots, observers, or callbacks

### 2.3 Qt/Framework Boundary Separation

To maintain modularity and testability, Qt types are confined to the presentation layer. Core logic uses standard C++20 types exclusively.

```
┌─────────────────────────────────────────────────────────────────┐
│  GUI Layer (Qt Widgets)                                         │
│  - Owns QString, QWidget, QSettings                             │
│  - Converts std::string → QString at display time               │
│  - Converts QString → std::string when passing to lower layers  │
├─────────────────────────────────────────────────────────────────┤
│  Render Layer (QOpenGLWidget)                                   │
│  - Inherits Qt classes (unavoidable for OpenGL context)         │
│  - Signals/slots use std::string, int32_t, custom structs       │
├─────────────────────────────────────────────────────────────────┤
│  Simulation/Domain Layers (Pure C++20)                          │
│  - std::string, std::vector, std::optional, glm types           │
│  - Zero Qt headers                                              │
├─────────────────────────────────────────────────────────────────┤
│  Database Layer (Abstracted)                                    │
│  - Interface uses std::string, std::vector<uint8_t>             │
│  - Qt SQL hidden in implementation                              │
└─────────────────────────────────────────────────────────────────┘
```

#### Type Mapping at Boundaries

| Layer | String | Container | Optional | Time |
|-------|--------|-----------|----------|------|
| GUI | `QString` | `QList` | `QVariant` | `QDateTime` |
| Boundary | Convert ↕ | Convert ↕ | Convert ↕ | Convert ↕ |
| Core | `std::string` | `std::vector` | `std::optional` | `std::chrono` |
| Database | `std::string` | `std::vector` | `std::optional` | `int64_t` (unix) |

#### Pattern: Abstract Interface for Qt-Dependent Services

Define pure C++ interfaces in core layers; implement with Qt in the GUI layer.

```cpp
// core/core_settings_interface.h - Pure C++ interface
class ISettings
{
public:
    virtual ~ISettings() = default;
    virtual void SetString(const std::string& key, const std::string& value) = 0;
    virtual std::optional<std::string> GetString(const std::string& key) const = 0;
    virtual void SetInt(const std::string& key, int32_t value) = 0;
    virtual std::optional<int32_t> GetInt(const std::string& key) const = 0;
};

// gui/gui_qt_settings.h - Qt implementation (only in GUI layer)
class QtSettings : public Core::ISettings
{
public:
    void SetString(const std::string& key, const std::string& value) override
    {
        m_Settings.setValue(QString::fromStdString(key),
                           QString::fromStdString(value));
    }

    std::optional<std::string> GetString(const std::string& key) const override
    {
        QVariant val = m_Settings.value(QString::fromStdString(key));
        if (!val.isValid()) return std::nullopt;
        return val.toString().toStdString();
    }

private:
    QSettings m_Settings{"Orogena", "Orogena"};
};
```

#### Pattern: Signals Use Standard Types

When Qt classes must emit signals (e.g., `QOpenGLWidget`), use `std::string` instead of `QString`:

```cpp
// In render/render_viewport.h
signals:
    void FPSUpdated(int32_t fps);
    void OpenGLInitialized(const std::string& vendor, const std::string& renderer,
                           const std::string& version);
    void OpenGLError(const std::string& error);

// In gui/gui_main_window.cpp - GUI layer converts at the boundary
connect(m_Viewport, &Render::Viewport::OpenGLError, this,
        [this](const std::string& error)
        {
            QMessageBox::critical(this, "OpenGL Error",
                                  QString::fromStdString(error));
        });
```

#### Pattern: Callback Interfaces for Non-Qt Components

For simulation components that need to report progress without Qt dependency:

```cpp
// global/global_simulation_callbacks.h - Pure C++
struct SimulationCallbacks
{
    std::function<void(int32_t step, int32_t totalSteps)> onProgress;
    std::function<void(const std::string& message)> onStatusUpdate;
    std::function<void(const std::string& error)> onError;
    std::function<void()> onComplete;
};

// gui/gui_simulation_controller.cpp - Bridge to Qt signals
void SimulationController::StartSimulation()
{
    Global::SimulationCallbacks callbacks;

    callbacks.onProgress = [this](int32_t step, int32_t total) {
        emit ProgressUpdated(step, total);  // Qt signal
    };

    callbacks.onError = [this](const std::string& error) {
        emit ErrorOccurred(QString::fromStdString(error));
    };

    m_Simulation.SetCallbacks(callbacks);
    m_Simulation.Run();
}
```

#### CMake Enforcement

Ensure core libraries don't link Qt:

```cmake
# Core libraries - NO Qt dependencies
target_link_libraries(orogena_core
    PUBLIC
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)

# Simulation layers - NO Qt dependencies
target_link_libraries(orogena_global
    PUBLIC
        orogena_core
        glm::glm
)

# GUI layer - Qt dependencies live here
target_link_libraries(orogena_gui
    PUBLIC
        Qt6::Widgets
        orogena_render
)
```

#### Benefits

1. **Testability**: Core logic can be unit tested without Qt
2. **Portability**: Simulation code could run on a server or CLI tool
3. **Maintainability**: Changing UI framework only affects GUI layer
4. **Clarity**: Clear contracts between layers

---

## 3. Multi-Scale Architecture

### 3.1 Scale Separation

Each scale is **self-contained** with clear input/output contracts:

| Scale | Input | Output | Storage |
|-------|-------|--------|---------|
| **Scale 1** (Global) | User params (seed, plate count) | Global uplift map, plate data | Single planet record |
| **Scale 2** (Regional) | Uplift map + neighbor borders | Regional heightmap + rivers | Grid-aligned tiles (100×100km) |
| **Scale 3** (Local) | Regional heightmap + geology | High-detail terrain | Generated on-the-fly |

### 3.2 Data Flow Between Scales

```cpp
// Example: Scale 1 → Scale 2 data flow

// Scale 1 produces global uplift map
class TectonicSimulation {
public:
    GlobalUpliftMap generateUpliftMap();
};

// Scale 2 consumes uplift map for region
class RegionManager {
public:
    RegionalHeightMap generateRegion(
        const GlobalUpliftMap& upliftMap,
        const RegionBounds& bounds,
        const BorderData& neighborBorders
    );
};

// Clear contract: Scale 2 doesn't know about plate physics,
// Scale 1 doesn't know about erosion details
```

### 3.3 Border Stitching Protocol

Regional tiles must stitch seamlessly. Use this protocol:

1. **Storage tiles**: Grid-aligned with fixed size (100×100km core + border overlap)
2. **Border overlap**: 5-10km on each edge
3. **Generation order**: Generate tiles with dependencies first (upstream for rivers)
4. **Border sharing**: Copy neighbor borders as boundary conditions

```cpp
class BorderStitcher {
public:
    /**
     * Extracts border data from a region for sharing with neighbors
     */
    BorderData extractBorder(
        const RegionalHeightMap& region,
        BorderDirection direction
    );

    /**
     * Applies neighbor border as boundary condition during generation
     */
    void applyBorderCondition(
        RegionalHeightMap& region,
        const BorderData& neighborBorder,
        BorderDirection direction
    );
};
```

---

## 4. Design Patterns

### 4.1 Required Patterns

#### **Manager Pattern**
Use for orchestrating complex operations across multiple components.

```cpp
/**
 * RegionManager orchestrates regional tile generation,
 * caching, and composition
 */
class RegionManager {
public:
    RegionalHeightMap getOrGenerateRegion(const RegionBounds& bounds);

private:
    TileCache m_cache;
    StreamPowerErosion m_erosionSim;
    BorderStitcher m_stitcher;
    ViewCompositor m_compositor;
};
```

**Use when**: Coordinating multiple related components

#### **Strategy Pattern**
Use for interchangeable algorithms (erosion types, noise generators).

```cpp
class ErosionStrategy {
public:
    virtual ~ErosionStrategy() = default;
    virtual void erode(HeightMap& map, double dt) = 0;
};

class StreamPowerErosion : public ErosionStrategy {
public:
    void erode(HeightMap& map, double dt) override;
};

class ThermalErosion : public ErosionStrategy {
public:
    void erode(HeightMap& map, double dt) override;
};
```

**Use when**: Multiple algorithms for the same operation

#### **Observer Pattern** (via Qt Signals/Slots)
Use for event notification without tight coupling.

```cpp
class TectonicSimulation : public QObject {
    Q_OBJECT

signals:
    void simulationStepCompleted(int stepNumber);
    void plateCollisionDetected(int plate1, int plate2);

    // Listeners don't need to know about TectonicSimulation internals
};
```

**Use when**: Notifying multiple components of state changes

#### **Factory Pattern**
Use for creating complex objects with multiple configurations.

```cpp
class PlateFactory {
public:
    static Plate createOceanicPlate(const glm::vec3& position);
    static Plate createContinentalPlate(const glm::vec3& position);
    static Plate createFromConfig(const PlateConfig& config);
};
```

**Use when**: Object creation is complex or has multiple variants

#### **RAII (Resource Acquisition Is Initialization)**
Use for all resource management (files, GPU resources, database connections).

```cpp
class GPUBuffer {
public:
    GPUBuffer(size_t size) {
        glGenBuffers(1, &m_bufferId);
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
    }

    ~GPUBuffer() {
        glDeleteBuffers(1, &m_bufferId);
    }

    // Delete copy, allow move
    GPUBuffer(const GPUBuffer&) = delete;
    GPUBuffer& operator=(const GPUBuffer&) = delete;
    GPUBuffer(GPUBuffer&&) = default;
    GPUBuffer& operator=(GPUBuffer&&) = default;

private:
    GLuint m_bufferId;
};
```

**Use when**: Managing any resource (always!)

### 4.2 Patterns to Avoid

#### **God Object**
Avoid classes that know or do too much.

```cpp
// BAD: Simulation does everything
class Simulation {
    void runSimulation();
    void renderScene();
    void saveToDatabase();
    void exportHeightmap();
    void handleUserInput();
    // ... 50 more responsibilities
};

// GOOD: Single responsibility per class
class TectonicSimulation { void runSimulation(); };
class Renderer { void renderScene(); };
class DatabaseManager { void saveProject(); };
class Exporter { void exportHeightmap(); };
```

#### **Singleton** (use sparingly)
Only use for truly global, single-instance resources.

```cpp
// Acceptable (but consider dependency injection instead)
class Logger {
public:
    static Logger& instance();
    void log(const std::string& message);
private:
    Logger() = default;
};

// Better: Dependency injection
class MyClass {
public:
    explicit MyClass(Logger& logger) : m_logger(logger) {}
private:
    Logger& m_logger;
};
```

#### **Premature Abstraction**
Don't create abstractions until you have multiple concrete implementations.

```cpp
// BAD: Abstract class with only one implementation
class IHeightMapStorage {
    virtual void save() = 0;
};
class SqliteHeightMapStorage : public IHeightMapStorage { };
// No other implementations exist or planned

// GOOD: Direct implementation, refactor to interface when needed
class HeightMapStorage {
    void save();
};
```

---

## 5. Component Design Guidelines

### 5.1 Class Design Principles

#### **Single Responsibility Principle**
Each class should have one reason to change.

```cpp
// BAD: PlateManager does too much
class PlateManager {
    void generatePlates();
    void simulatePhysics();
    void renderPlates();
    void savePlates();
};

// GOOD: Separate concerns
class PlateGenerator { void generatePlates(); };
class PlatePhysics { void simulate(); };
class PlateRenderer { void render(); };
class PlateSerializer { void save(); };
```

#### **Dependency Inversion Principle**
Depend on abstractions, not concrete implementations (when appropriate).

```cpp
// Simulation depends on abstract cache interface
class IRegionCache {
public:
    virtual ~IRegionCache() = default;
    virtual std::optional<Region> get(const RegionKey& key) = 0;
    virtual void put(const RegionKey& key, const Region& region) = 0;
};

class RegionManager {
public:
    explicit RegionManager(std::unique_ptr<IRegionCache> cache)
        : m_cache(std::move(cache)) {}

private:
    std::unique_ptr<IRegionCache> m_cache;
};

// Allows testing with mock cache, production with SQLite cache
```

#### **Interface Segregation**
Clients shouldn't depend on methods they don't use.

```cpp
// BAD: Fat interface
class ITerrainData {
    virtual float getHeight(int x, int y) = 0;
    virtual void setHeight(int x, int y, float h) = 0;
    virtual RiverNetwork getRivers() = 0;
    virtual void addRiver(const River& r) = 0;
    virtual GeologyData getGeology() = 0;
    // ... 20 more methods
};

// GOOD: Segregated interfaces
class IHeightMapReader {
    virtual float getHeight(int x, int y) const = 0;
};

class IHeightMapWriter {
    virtual void setHeight(int x, int y, float h) = 0;
};

class IRiverNetwork {
    virtual RiverNetwork getRivers() const = 0;
};
```

### 5.2 Class Structure Template

```cpp
class ComponentName {
public:
    // 1. Public type definitions
    enum class Mode { ... };
    struct Config { ... };

    // 2. Constructors (explicit for single-argument)
    ComponentName();
    explicit ComponentName(const Config& config);

    // 3. Rule of Five (or Zero)
    ~ComponentName();
    ComponentName(const ComponentName&) = delete;
    ComponentName& operator=(const ComponentName&) = delete;
    ComponentName(ComponentName&&) = default;
    ComponentName& operator=(ComponentName&&) = default;

    // 4. Primary interface (most important methods first)
    void primaryOperation();
    ResultType calculate();

    // 5. Getters/setters (group related)
    int getValue() const;
    void setValue(int value);

    // 6. Query methods (const)
    bool isReady() const;
    size_t getCount() const;

private:
    // 7. Private types
    struct InternalData { ... };

    // 8. Private helper methods
    void internalHelper();

    // 9. Member variables (m_ prefix, grouped logically)
    // Simple types first
    int m_value;
    bool m_initialized;

    // Complex types
    std::vector<Data> m_data;
    std::unique_ptr<Helper> m_helper;
};
```

---

## 6. Threading and Concurrency

### 6.1 Threading Model

Orogena uses a **hybrid threading model**:

1. **Main Thread**: UI event loop (Qt main thread)
2. **Render Thread**: OpenGL context and rendering (Qt render thread)
3. **Worker Threads**: Simulation, erosion, tile generation (QtConcurrent)
4. **GPU Threads**: Compute shader execution (GPU driver)

### 6.2 Thread Safety Rules

#### **Shared Data Access**
```cpp
class ThreadSafeCache {
public:
    void put(const Key& key, const Value& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_cache[key] = value;
    }

    std::optional<Value> get(const Key& key) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_cache.find(key);
        return it != m_cache.end() ? std::optional{it->second} : std::nullopt;
    }

private:
    mutable std::mutex m_mutex;
    std::unordered_map<Key, Value> m_cache;
};
```

#### **Async Operations**
```cpp
// Use QtConcurrent for CPU-bound work
QFuture<RegionalHeightMap> future = QtConcurrent::run([=]() {
    return m_regionManager.generateRegion(bounds);
});

// Connect completion to UI update
auto* watcher = new QFutureWatcher<RegionalHeightMap>(this);
connect(watcher, &QFutureWatcher<RegionalHeightMap>::finished,
        this, &MainWindow::onRegionReady);
watcher->setFuture(future);
```

#### **Immutability for Thread Safety**
```cpp
// Immutable data structures are inherently thread-safe
class GlobalUpliftMap {
public:
    explicit GlobalUpliftMap(std::vector<float> data)
        : m_data(std::move(data)) {}

    // Read-only interface
    float getUplift(int x, int y) const {
        return m_data[y * m_width + x];
    }

    // No setters - construct new instance for modifications

private:
    const std::vector<float> m_data;
    const int m_width;
    const int m_height;
};
```

### 6.3 GPU Synchronization

```cpp
class ComputeShaderSimulation {
public:
    void runSimulationStep() {
        // Dispatch compute shader
        glDispatchCompute(workGroupsX, workGroupsY, 1);

        // Ensure computation completes before CPU reads
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // Synchronize before using results
        glFinish(); // Or use fence sync for async
    }

    void runAsync() {
        glDispatchCompute(workGroupsX, workGroupsY, 1);

        // Create fence for async synchronization
        GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

        // Check later without blocking
        // GLenum result = glClientWaitSync(fence, 0, timeout);
    }
};
```

---

## 7. Error Handling Strategy

### 7.1 Error Categories

| Category | Handling | Example |
|----------|----------|---------|
| **Programming errors** | Assert/abort | Null pointer, invalid index |
| **User errors** | Exception + UI message | Invalid plate count |
| **Resource errors** | Exception + recovery | File not found, out of memory |
| **Expected failures** | Return code/optional | Cache miss, optional feature |

### 7.2 Error Handling Patterns

#### **Use `std::optional` for expected failures**
```cpp
std::optional<Region> RegionCache::get(const RegionKey& key) {
    auto it = m_cache.find(key);
    if (it != m_cache.end()) {
        return it->second;
    }
    return std::nullopt; // Cache miss is expected
}

// Usage
if (auto region = m_cache.get(key)) {
    use(*region);
} else {
    generateRegion(key);
}
```

#### **Use exceptions for unexpected errors**
```cpp
void DatabaseManager::connect(const QString& path) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open()) {
        throw std::runtime_error(
            std::format("Failed to open database: {}",
                       m_db.lastError().text().toStdString())
        );
    }
}
```

#### **Use assertions for programming errors**
```cpp
void setHeight(int x, int y, float value) {
    assert(x >= 0 && x < m_width && "X coordinate out of bounds");
    assert(y >= 0 && y < m_height && "Y coordinate out of bounds");

    m_data[y * m_width + x] = value;
}
```

#### **Use result types for detailed errors**
```cpp
struct ExportResult {
    bool success;
    QString errorMessage;
    QString outputPath;
};

ExportResult exportHeightMap(const QString& path) {
    if (!validatePath(path)) {
        return {false, "Invalid file path", ""};
    }

    if (!writeFile(path, data)) {
        return {false, "Failed to write file", ""};
    }

    return {true, "", path};
}
```

---

## 8. Performance Design

### 8.1 Performance Targets

| Operation | Target | Strategy |
|-----------|--------|----------|
| Global simulation step | <5s @ 2048² | GPU compute shaders |
| Regional tile generation | <30s @ 100×100km | CPU/GPU hybrid |
| Local detail synthesis | <1s | CPU procedural |
| Rendering (global view) | 60 FPS | LOD, frustum culling |
| Cache lookup | <10ms | Indexed database |

### 8.2 Optimization Strategy

#### **1. Profile First**
Never optimize without profiling data.

```cpp
// Use scoped profiling for hot paths
class ScopedProfiler {
public:
    explicit ScopedProfiler(const std::string& name)
        : m_name(name), m_start(std::chrono::high_resolution_clock::now()) {}

    ~ScopedProfiler() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end - m_start
        );
        spdlog::info("{} took {}ms", m_name, duration.count());
    }

private:
    std::string m_name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};

void expensiveOperation() {
    ScopedProfiler profiler("expensiveOperation");
    // ... implementation
}
```

#### **2. Cache Aggressively**
Store expensive computation results.

```cpp
class RegionManager {
public:
    RegionalHeightMap getRegion(const RegionKey& key) {
        // Check memory cache first
        if (auto region = m_memoryCache.get(key)) {
            return *region;
        }

        // Check disk cache
        if (auto region = m_diskCache.get(key)) {
            m_memoryCache.put(key, *region);
            return *region;
        }

        // Generate and cache
        auto region = generateRegion(key);
        m_memoryCache.put(key, region);
        m_diskCache.put(key, region);
        return region;
    }

private:
    LRUCache<RegionKey, RegionalHeightMap> m_memoryCache{100}; // Keep 100 regions
    DatabaseCache m_diskCache;
};
```

#### **3. GPU Acceleration**
Offload parallel work to GPU.

```cpp
class ErosionSimulator {
public:
    void erode(HeightMap& map, int iterations) {
        if (m_useGPU && m_gpuAvailable) {
            erodeGPU(map, iterations);
        } else {
            erodeCPU(map, iterations);
        }
    }

private:
    void erodeGPU(HeightMap& map, int iterations);  // 50x faster
    void erodeCPU(HeightMap& map, int iterations);  // Fallback

    bool m_useGPU;
    bool m_gpuAvailable;
};
```

#### **4. Lazy Evaluation**
Delay computation until needed.

```cpp
class LocalDetailSynthesizer {
public:
    // Don't generate detail until user zooms in
    void onZoomLevelChanged(float zoom) {
        if (zoom > kLocalDetailThreshold && !m_detailGenerated) {
            generateLocalDetail();
            m_detailGenerated = true;
        }
    }

private:
    void generateLocalDetail();
    bool m_detailGenerated = false;
};
```

### 8.3 Memory Management

#### **Bounded Memory Usage**
```cpp
// LRU cache with max size
template<typename K, typename V>
class LRUCache {
public:
    explicit LRUCache(size_t maxSize) : m_maxSize(maxSize) {}

    void put(const K& key, const V& value) {
        if (m_cache.size() >= m_maxSize) {
            evictOldest();
        }
        m_cache[key] = value;
    }

private:
    void evictOldest();

    size_t m_maxSize;
    std::unordered_map<K, V> m_cache;
};
```

#### **Memory Pooling for Frequent Allocations**
```cpp
// For frequently created/destroyed objects
class PlateCollisionPool {
public:
    CollisionEvent* acquire() {
        if (m_pool.empty()) {
            return new CollisionEvent();
        }
        auto* event = m_pool.back();
        m_pool.pop_back();
        return event;
    }

    void release(CollisionEvent* event) {
        event->reset();
        m_pool.push_back(event);
    }

private:
    std::vector<CollisionEvent*> m_pool;
};
```

---

## 9. Database Design

### 9.1 Schema Design Principles

1. **Normalized for planets table** (single source of truth)
2. **Denormalized for storage_regions** (optimize for read performance)
3. **Use BLOBs for large binary data** (heightmaps, geometry)
4. **Index frequently queried columns**
5. **Use transactions for consistency**

### 9.2 Data Access Pattern

```cpp
// Use DAO (Data Access Object) pattern
class RegionDAO {
public:
    explicit RegionDAO(DatabaseManager& db) : m_db(db) {}

    std::optional<Region> findByGridPosition(
        const QString& planetId,
        int gridX,
        int gridY
    ) {
        QSqlQuery query(m_db.database());
        query.prepare(
            "SELECT * FROM storage_regions "
            "WHERE planet_id = ? AND grid_x = ? AND grid_y = ?"
        );
        query.addBindValue(planetId);
        query.addBindValue(gridX);
        query.addBindValue(gridY);

        if (query.exec() && query.next()) {
            return deserializeRegion(query);
        }

        return std::nullopt;
    }

    void save(const Region& region) {
        // Implementation
    }

private:
    Region deserializeRegion(const QSqlQuery& query);

    DatabaseManager& m_db;
};
```

### 9.3 Transaction Management

```cpp
class DatabaseTransaction {
public:
    explicit DatabaseTransaction(QSqlDatabase& db)
        : m_db(db), m_committed(false)
    {
        m_db.transaction();
    }

    ~DatabaseTransaction() {
        if (!m_committed) {
            m_db.rollback();
        }
    }

    void commit() {
        m_db.commit();
        m_committed = true;
    }

private:
    QSqlDatabase& m_db;
    bool m_committed;
};

// Usage
void saveMultipleRegions(const std::vector<Region>& regions) {
    DatabaseTransaction transaction(m_db);

    for (const auto& region : regions) {
        m_regionDAO.save(region);
    }

    transaction.commit(); // All or nothing
}
```

---

## 10. Testing Strategy

### 10.1 Test Pyramid

```
      ┌─────────┐
      │   E2E   │  ← Few (integration tests)
      └─────────┘
    ┌─────────────┐
    │   Component │  ← Some (module tests)
    └─────────────┘
  ┌─────────────────┐
  │      Unit       │  ← Many (function/class tests)
  └─────────────────┘
```

### 10.2 Unit Testing

```cpp
// Test individual components in isolation
TEST(VoronoiTest, GeneratesCorrectNumberOfCells) {
    VoronoiSphere voronoi;
    std::vector<glm::vec3> seeds = generateRandomSeeds(15);

    auto diagram = voronoi.generate(seeds);

    EXPECT_EQ(diagram.cells().size(), 15);
}

TEST(StreamPowerErosionTest, ConvergesToStableState) {
    HeightMap map = createTestMountain();
    StreamPowerErosion erosion;

    // Run until convergence
    for (int i = 0; i < 1000; ++i) {
        erosion.step(map, 0.01);
    }

    // Check stability
    float changeBefore = computeTotalChange(map);
    erosion.step(map, 0.01);
    float changeAfter = computeTotalChange(map);

    EXPECT_LT(changeAfter / changeBefore, 0.01); // <1% change
}
```

### 10.3 Component Testing

```cpp
// Test integration between components
TEST(RegionManagerTest, StitchesAdjacentTilesSeamlessly) {
    RegionManager manager;

    auto region1 = manager.generateRegion(bounds1);
    auto region2 = manager.generateRegion(bounds2); // Adjacent to region1

    // Check border continuity
    for (int i = 0; i < borderSize; ++i) {
        float height1 = region1.getHeight(edgeX, i);
        float height2 = region2.getHeight(0, i);
        EXPECT_NEAR(height1, height2, 0.01f);
    }
}
```

### 10.4 Mock Objects for Testing

```cpp
// Mock cache for testing without database
class MockRegionCache : public IRegionCache {
public:
    std::optional<Region> get(const RegionKey& key) override {
        auto it = m_storage.find(key);
        return it != m_storage.end() ? std::optional{it->second} : std::nullopt;
    }

    void put(const RegionKey& key, const Region& region) override {
        m_storage[key] = region;
    }

private:
    std::unordered_map<RegionKey, Region> m_storage;
};

// Use in tests
TEST(RegionManagerTest, CachesGeneratedRegions) {
    auto mockCache = std::make_unique<MockRegionCache>();
    auto* cachePtr = mockCache.get();

    RegionManager manager(std::move(mockCache));

    auto region = manager.getOrGenerateRegion(testBounds);

    EXPECT_TRUE(cachePtr->get(testBounds).has_value());
}
```

---

## 11. Documentation Standards

### 11.1 Code Documentation

- **Public API**: Full Doxygen documentation
- **Private implementation**: Brief comments explaining non-obvious logic
- **Complex algorithms**: Detailed comments with references

```cpp
/**
 * @brief Calculates stream power erosion for a heightmap region
 *
 * Implements the stream power erosion model based on the equation:
 * dh/dt = U - K * A^m * S^n
 *
 * Where:
 * - dh/dt: Rate of elevation change
 * - U: Uplift rate
 * - K: Erosion coefficient
 * - A: Drainage area
 * - S: Local slope
 * - m, n: Empirical constants (typically m=0.5, n=1.0)
 *
 * Algorithm:
 * 1. Calculate drainage area using D8 flow routing
 * 2. Compute local slopes
 * 3. Apply erosion equation to each cell
 * 4. Handle base level boundary conditions
 *
 * @param heightMap The terrain heightmap to erode (modified in place)
 * @param params Erosion parameters (K, m, n, timestep)
 * @param iterations Number of erosion steps to perform
 *
 * @throws std::invalid_argument if heightMap is empty or params are invalid
 *
 * @see Braun & Willett (2013) "A very efficient O(n), implicit and parallel
 *      method to solve the stream power equation"
 *
 * @note This is a simplified model that doesn't account for sediment
 *       transport or lithology variations
 */
void erode(HeightMap& heightMap, const ErosionParams& params, int iterations);
```

### 11.2 Architecture Documentation

Maintain these documents in `docs/`:
- `001_sdp_orogena.md`: Software Development Plan (already created)
- `002_coding_standard.md`: This document
- `003_design_standard.md`: Design patterns and architecture
- `004_api_reference.md`: Public API documentation (generated)
- `005_performance_guide.md`: Optimization tips and profiling results

---

## 12. Versioning and Compatibility

### 12.1 Semantic Versioning

Follow SemVer: `MAJOR.MINOR.PATCH`

- **MAJOR**: Incompatible API/file format changes
- **MINOR**: New features, backward compatible
- **PATCH**: Bug fixes, backward compatible

### 12.2 File Format Versioning

```cpp
struct ProjectFileHeader {
    uint32_t magic = 0x4F524F47; // 'OROG'
    uint16_t majorVersion = 1;
    uint16_t minorVersion = 0;
    // ...
};

class ProjectLoader {
public:
    Project load(const QString& path) {
        auto header = readHeader(path);

        if (header.majorVersion != kCurrentMajorVersion) {
            throw std::runtime_error("Incompatible project file version");
        }

        if (header.minorVersion < kCurrentMinorVersion) {
            return loadAndMigrate(path, header);
        }

        return loadCurrent(path);
    }

private:
    Project loadAndMigrate(const QString& path, const ProjectFileHeader& header);
};
```

---

## 13. Platform Considerations

### 13.1 Cross-Platform Code

```cpp
// Use Qt's platform abstraction
QString getDataDirectory() {
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

// Platform-specific code when necessary
#ifdef Q_OS_WIN
    constexpr const char* kPathSeparator = "\\";
#else
    constexpr const char* kPathSeparator = "/";
#endif

// Prefer cross-platform libraries
// Good: Qt, GLM, spdlog
// Avoid: Windows-only APIs, platform-specific filesystem code
```

### 13.2 OpenGL vs Vulkan

```cpp
// Abstract graphics API behind interface
class IGraphicsContext {
public:
    virtual ~IGraphicsContext() = default;
    virtual void initialize() = 0;
    virtual void createBuffer(size_t size) = 0;
    // ...
};

class OpenGLContext : public IGraphicsContext { /* ... */ };
class VulkanContext : public IGraphicsContext { /* ... */ };

// Factory creates appropriate context for platform
std::unique_ptr<IGraphicsContext> createGraphicsContext() {
    #ifdef OROGENA_USE_VULKAN
        return std::make_unique<VulkanContext>();
    #else
        return std::make_unique<OpenGLContext>();
    #endif
}
```

---

## 14. Security Considerations

### 14.1 Input Validation

```cpp
void loadProject(const QString& path) {
    // Validate file path
    QFileInfo fileInfo(path);
    if (!fileInfo.exists() || !fileInfo.isReadable()) {
        throw std::runtime_error("File not accessible");
    }

    // Validate file size (prevent DoS)
    if (fileInfo.size() > kMaxProjectFileSize) {
        throw std::runtime_error("Project file too large");
    }

    // Validate magic number
    auto header = readHeader(path);
    if (header.magic != kProjectFileMagic) {
        throw std::runtime_error("Invalid project file format");
    }

    // ... continue loading
}
```

### 14.2 Resource Limits

```cpp
class SimulationConfig {
public:
    void setPlateCount(int count) {
        if (count < kMinPlateCount || count > kMaxPlateCount) {
            throw std::invalid_argument(
                std::format("Plate count must be between {} and {}",
                           kMinPlateCount, kMaxPlateCount)
            );
        }
        m_plateCount = count;
    }

private:
    static constexpr int kMinPlateCount = 5;
    static constexpr int kMaxPlateCount = 50;
    static constexpr size_t kMaxGridResolution = 4096;
    static constexpr size_t kMaxMemoryUsage = 2ULL * 1024 * 1024 * 1024; // 2GB
};
```

---

## 15. Extensibility and Future-Proofing

### 15.1 Plugin Architecture (Future)

Design components to be pluggable:

```cpp
// Define stable interfaces for future plugins
class IErosionAlgorithm {
public:
    virtual ~IErosionAlgorithm() = default;
    virtual QString name() const = 0;
    virtual QString version() const = 0;
    virtual void erode(HeightMap& map, const QVariantMap& params) = 0;
};

// Plugin manager (for future implementation)
class PluginManager {
public:
    void registerErosionAlgorithm(std::unique_ptr<IErosionAlgorithm> algorithm);
    IErosionAlgorithm* getAlgorithm(const QString& name);

private:
    std::unordered_map<QString, std::unique_ptr<IErosionAlgorithm>> m_algorithms;
};
```

### 15.2 Configuration Over Code

```cpp
// Use JSON for configurations that might change
struct ErosionConfig {
    double erosionCoefficient;
    double upliftRate;
    int iterations;

    static ErosionConfig fromJson(const QJsonObject& json);
    QJsonObject toJson() const;
};

// Load from file
auto config = ErosionConfig::fromJson(loadJsonFile("erosion_preset.json"));

// Allows users to create custom presets without code changes
```

---

## 16. Review Checklist

Before merging code, verify:

**Architecture**:
- [ ] Follows layered architecture (no layer violations)
- [ ] Components have single, clear responsibility
- [ ] Dependencies point downward only
- [ ] Uses appropriate design patterns

**Performance**:
- [ ] Hot paths profiled and optimized
- [ ] Caching used where appropriate
- [ ] No unnecessary allocations in loops
- [ ] GPU acceleration for parallel work

**Thread Safety**:
- [ ] Shared data properly synchronized
- [ ] No data races (verified with sanitizer)
- [ ] Immutable data used where possible

**Error Handling**:
- [ ] Errors handled at appropriate level
- [ ] User-facing errors have clear messages
- [ ] Resources cleaned up via RAII
- [ ] No swallowed exceptions

**Testing**:
- [ ] Unit tests for new functionality
- [ ] Integration tests for component interactions
- [ ] Edge cases covered
- [ ] Performance tests for critical paths

**Documentation**:
- [ ] Public API fully documented
- [ ] Complex algorithms explained
- [ ] Architecture decisions recorded

---

**Document Status**: Living document - will evolve with the project

**Next Steps**: Review with team, establish code review process, set up automated checks
