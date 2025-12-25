# Orogena

## Multi-Scale Tectonic Simulator for Fantasy World Building

*Software Development Plan*

Version 2.0 | December 2025

---

## 1. Project Overview

### 1.1 Vision

Create a professional-grade multi-scale tectonic simulation tool that enables fantasy authors, game developers, and world-builders to generate geologically plausible terrain from planetary scale down to local detail through an intuitive, visually rich interface. The application will simulate millions of years of plate tectonics at global scale, then allow seamless extraction of regional and local maps with progressive detail enhancement.

### 1.2 Key Innovation: Multi-Scale Architecture

Orogena uses a hierarchical three-scale approach:

- **Scale 1 (Global)**: Planet-wide tectonic simulation (~10-50km resolution)
- **Scale 2 (Regional)**: High-detail erosion simulation (~1-5km resolution)
- **Scale 3 (Local)**: Ultra-fine procedural detail (~10-100m resolution)

*This architecture solves the fundamental challenge of seamless map generation from global to local scales, using border-sharing and hierarchical inheritance to eliminate seams between regions while maintaining computational feasibility.*

### 1.3 Core Features

- Seamless multi-scale generation: planet → continent → region → local detail
- GPU-accelerated global tectonic simulation with simplified convection forces
- Intelligent regional tile system with automatic border stitching
- On-demand generation of arbitrary view regions at any resolution
- Stream power erosion with realistic drainage network formation
- Multi-octave procedural detail with geology-aware noise synthesis
- Database-backed region caching for instant revisits
- Professional export: heightmaps, regional maps, geological history data
- Cross-platform support (Linux, Windows, macOS)

### 1.4 Target Platforms

- **Primary**: Linux (CachyOS) - Development and testing platform
- **Windows 10/11** - Primary target audience
- **macOS** (Intel and Apple Silicon) - Secondary platform

---

## 2. Technology Stack

### 2.1 Core Framework

- C++20 with Qt 6.8 for cross-platform UI and rendering
- OpenGL 4.5+ / Vulkan for GPU-accelerated simulation
- Compute shaders for parallel tectonic and erosion processing

### 2.2 Key Libraries

- **Qt 6.8**: UI framework, OpenGL integration, threading (QtConcurrent)
- **GLM**: Mathematics library for 3D transformations and physics
- **FastNoise2**: High-performance noise generation for procedural detail
- **SQLite**: Regional tile caching database
- **spdlog**: High-performance logging
- **nlohmann/json**: Project serialization and settings
- **stb_image_write**: Height map export (16-bit PNG)
- **boost::geometry**: Spatial calculations and region queries

---

## 3. System Architecture

### 3.1 Multi-Scale Data Flow

The system operates in three distinct scales, each feeding data to the next level:

**Global Scale (Scale 1)**
- Input: User parameters (plate count, seed, time duration)
- Process: GPU-accelerated simplified plate physics
- Output: Global uplift map, plate history, crust age/type
- Storage: Single global state per planet (~64MB)

**Regional Scale (Scale 2)**
- Input: Uplift data from Scale 1 + neighbor borders
- Process: Stream power erosion, drainage network formation
- Output: High-resolution heightmap with rivers/valleys
- Storage: Grid-aligned 100×100km tiles cached in database

**Local Scale (Scale 3)**
- Input: Regional heightmap + geology data
- Process: Multi-octave noise, fine erosion detail
- Output: Ultra-high-res terrain with surface features
- Storage: Generated on-the-fly, not cached

### 3.2 Border Stitching System

Regional tiles use overlapping borders to ensure seamless continuity:

- Storage regions: 100×100km core + 5-10km border overlap on each side
- When generating adjacent tiles, copy neighbor borders as boundary conditions
- Rivers and erosion patterns flow naturally across tile boundaries
- View regions: Arbitrary rectangles composited from storage tiles on-demand

### 3.3 Core Components

**Global Simulation Layer**
- TectonicSimulation: Global plate physics orchestrator
- PlateManager: Voronoi tessellation, plate creation, tracking
- SimplifiedConvectionEngine: Force-driven plate motion (slab pull, ridge push)
- GlobalPhysicsEngine: Collision detection, stress, velocity fields
- GeologyEngine: Subduction, orogeny, rifting, hotspots
- UpliftMapGenerator: Converts simulation state to uplift map

**Regional Generation Layer**
- RegionManager: Tile generation orchestration and caching
- StreamPowerErosion: Hydraulic erosion with drainage networks
- BorderStitcher: Ensures seamless tile boundaries
- TileCache: SQLite-backed persistent storage
- ViewCompositor: Assembles arbitrary regions from tiles

**Local Detail Layer**
- DetailSynthesizer: Multi-octave noise generation
- GeologyAwareNoise: Different patterns for rock types
- FineErosionSimulator: Sub-kilometer erosion effects
- ProceduralTextureGenerator: Surface detail synthesis

**Rendering Layer**
- MultiScaleRenderer: Handles global, regional, and local views
- LODManager: Level-of-detail based on camera distance
- HeightMapRenderer: 2D and 3D visualization with shading
- PlateVisualization: Boundaries, velocities, stress overlays
- Camera: Seamless zoom from planet to ground level

**UI Layer**
- MainWindow: Multi-viewport support for different scales
- GlobalViewport: Planet-scale view and simulation controls
- RegionalViewport: Continent/regional map extraction
- LocalViewport: Ground-level detail preview
- ExportDialog: Multi-scale export options
- CacheManager UI: Tile management and cleanup tools

---

## 4. Database Schema

SQLite database manages planet state and regional tile caching:

### planets table

```sql
CREATE TABLE planets (
    planet_id TEXT PRIMARY KEY,
    seed INTEGER NOT NULL,
    name TEXT,
    created_at TIMESTAMP,
    tectonic_history BLOB,
    global_uplift_map BLOB,
    plate_configuration BLOB,
    resolution_km INTEGER,
    metadata JSON
);
```

### storage_regions table

```sql
CREATE TABLE storage_regions (
    region_id TEXT PRIMARY KEY,
    planet_id TEXT REFERENCES planets(planet_id),
    grid_x INTEGER,
    grid_y INTEGER,
    size_km INTEGER,
    resolution_m INTEGER,
    heightmap BLOB,
    river_network BLOB,
    geology_data BLOB,
    border_data BLOB,
    generated_at TIMESTAMP,
    UNIQUE(planet_id, grid_x, grid_y)
);

CREATE INDEX idx_region_lookup
    ON storage_regions(planet_id, grid_x, grid_y);
```

---

## 5. Development Phases

The project is divided into nine phases, building from foundation to complete multi-scale system. Estimated total development time is 20-28 weeks for a solo developer with C++/Qt experience.

### Phase 1: Foundation & Infrastructure (Weeks 1-2)

**Goal**: Establish project infrastructure, build system, and database foundation.

- Set up CMake project with vcpkg/Conan for dependency management
- Configure cross-platform builds (Linux, Windows, macOS)
- Create basic Qt MainWindow with menu bar and dockable panel framework
- Implement SQLite database wrapper with connection pooling
- Set up logging system (spdlog) with file rotation
- Create configuration management system using QSettings
- Implement basic QOpenGLWidget viewport with empty scene
- Set up project save/load infrastructure with JSON serialization

**Deliverable**: Running application with empty viewport, functional database, and save system

### Phase 2: Global Grid & Rendering (Weeks 3-4)

**Goal**: Implement global heightmap grid and basic visualization (Scale 1 foundation).

- Design GlobalHeightMap class with configurable resolution (512² to 4096²)
- Implement sphere-to-grid mapping (equirectangular projection)
- Create basic noise generation for initial terrain testing
- Build 2D visualization mode with color-mapped elevation
- Implement 3D mesh generation from global heightmap
- Add orbit camera with mouse controls (rotate, pan, zoom)
- Create resolution selector and random seed controls
- Implement GPU upload pipeline for large heightmap data

**Deliverable**: Interactive 3D globe viewer with procedural terrain

### Phase 3: Voronoi Plate Generation (Weeks 5-6)

**Goal**: Implement tectonic plate generation using Voronoi tessellation on sphere.

- Implement spherical Voronoi diagram (adapt Fortune's algorithm or use boost::geometry)
- Create Plate class with type (oceanic/continental), velocity vector, age
- Build PlateManager for creation, tracking, and merging
- Assign global grid cells to plates using nearest-neighbor queries
- Visualize plates with distinct colors and highlighted boundaries
- Add plate count slider (5-50 plates) with regeneration
- Display velocity vectors as arrows on plate visualization
- Implement plate selection and property editing in UI

**Deliverable**: Configurable plate generation with interactive debugging

### Phase 4: Simplified Plate Physics (Weeks 7-9)

**Goal**: Implement force-driven plate motion without full mantle convection.

- Implement rotation-based plate movement using rotation poles and angular velocity
- Create force calculation system: slab pull, ridge push, collision resistance
- Detect plate boundary types: convergent, divergent, transform
- Track crust age and thickness per cell
- Implement stress accumulation at plate boundaries
- Add collision response: uplift at convergent boundaries
- Create divergent boundary behavior: rifting, new oceanic crust
- Build simulation controls: play, pause, step, speed, time display
- Implement simulation state snapshots for timeline scrubbing

**Deliverable**: Animated plate movement with realistic collision physics

### Phase 5: Geological Processes (Weeks 10-12)

**Goal**: Add realistic geological features and complete global simulation.

- Implement subduction zones with oceanic trench formation
- Create orogeny (mountain building) at continental collision boundaries
- Add volcanic arc generation parallel to subduction zones
- Implement hotspot volcanism independent of plate boundaries
- Create island chain formation from oceanic hotspots
- Add rift valley formation and flooding to create inland seas
- Implement continental shelf generation at coastlines
- Generate global uplift map output from final simulation state
- Add plate history tracking for geological timeline visualization

**Deliverable**: Complete global tectonic simulation with geologically plausible features

### Phase 6: Regional Tile System (Weeks 13-15)

**Goal**: Implement Scale 2 regional generation with seamless borders.

- Design RegionManager class with grid-aligned tile storage
- Implement tile generation from global uplift data
- Create border overlap system (5-10km on each edge)
- Build border data extraction and storage
- Implement neighbor detection and border sharing protocol
- Create SQLite-backed tile caching system
- Implement ViewRegion class for arbitrary area extraction
- Build compositor to stitch tiles into custom views
- Add spatial indexing (R-tree) for efficient tile queries
- Create region selection UI tool in viewport

**Deliverable**: Working tile system with seamless regional extraction

### Phase 7: Stream Power Erosion (Weeks 16-18)

**Goal**: Implement regional-scale erosion with realistic drainage networks.

- Implement stream power equation solver on heightmap
- Create drainage area calculation using flow accumulation
- Build river network extraction from flow paths
- Add thermal erosion for slope stability
- Implement sediment transport and deposition
- Create erosion parameter controls (uplift rate, erosion coefficient)
- Optimize erosion simulation for 100×100km regions (target: <30 seconds)
- Implement boundary condition handling for tile edges
- Add erosion preview mode before committing changes
- Create erosion visualization overlays (flow paths, drainage basins)

**Deliverable**: Realistic river valleys and drainage patterns in regional maps

### Phase 8: GPU Acceleration (Weeks 19-21)

**Goal**: Move simulation to GPU compute shaders for performance.

- Design compute shader architecture for global simulation
- Implement plate ownership update in compute shader
- Port collision detection and force calculation to GPU
- Create parallel heightmap modification shaders
- Implement GPU-based erosion simulation
- Add compute shader for drainage area calculation
- Optimize memory transfers between CPU and GPU
- Implement CPU fallback for systems without compute support
- Profile and optimize for target 60 FPS at 2048² global resolution
- Add progress reporting for long-running GPU operations

**Deliverable**: 10-50x performance improvement enabling real-time large-scale simulation

### Phase 9: Local Detail & Polish (Weeks 22-24)

**Goal**: Complete Scale 3 generation and finalize export features.

- Implement multi-octave noise generator with FastNoise2
- Create geology-aware noise patterns (sedimentary, igneous, metamorphic)
- Add fine-scale erosion detail (gullies, scree slopes)
- Implement local vegetation/biome influence on terrain
- Build multi-format export system (PNG, TIFF, raw binary)
- Add metadata export (JSON with geological properties)
- Create batch export for large regions
- Implement map projection options (Mercator, equirectangular, orthographic)
- Add screenshot and render-to-file functionality
- Polish UI with tooltips, status messages, and help system

**Deliverable**: Complete multi-scale terrain generation pipeline with professional export

---

## 6. Quality Assurance & Testing

### 6.1 Testing Strategy

- Unit tests for core algorithms (Voronoi, physics, erosion)
- Integration tests for multi-scale pipeline
- Visual regression tests for rendering output
- Performance benchmarks for all scales
- Cross-platform testing on Linux, Windows, macOS

### 6.2 Validation Criteria

- Global simulation: Realistic plate behavior, no artifacts
- Regional generation: Seamless tile boundaries, no discontinuities
- Erosion: Natural-looking drainage networks, stable convergence
- Performance: <5s for global step, <30s for regional tile, <1s for local detail
- Memory: <2GB for typical planet, graceful degradation on large projects

---

## 7. Future Extensions (Post v1.0)

### 7.1 Advanced Features

- Climate simulation influencing erosion patterns
- Sedimentary layer tracking for realistic geology
- Interactive terrain editing with physics preservation
- Multi-planet projects with shared geological parameters
- Animation export (time-lapse of tectonic evolution)

### 7.2 Community Features

- Planet sharing marketplace
- Preset library for different world types (Earth-like, high-gravity, etc.)
- Plugin system for custom erosion algorithms
- Scripting API for procedural generation workflows

---

## 8. Success Metrics

- **Performance**: Real-time global simulation at 2048² resolution
- **Quality**: Indistinguishable tile boundaries in regional maps
- **Usability**: Complete planet-to-local workflow in <10 minutes
- **Compatibility**: 90%+ success rate on target platforms
- **Adoption**: 100+ active users within 6 months of release

---

## 9. Risk Mitigation

### 9.1 Technical Risks

| Risk | Mitigation |
|------|------------|
| GPU compatibility issues | CPU fallback implementation |
| Tile boundary artifacts | Extensive testing, overlap tuning |
| Performance bottlenecks | Early profiling, incremental optimization |
| Database scaling | Efficient indexing, compression, cleanup tools |

### 9.2 Project Risks

| Risk | Mitigation |
|------|------------|
| Scope creep | Strict phase boundaries, MVP first |
| Platform-specific bugs | Continuous integration testing |
| User learning curve | Comprehensive tutorials, example projects |
| Maintenance burden | Modular architecture, good documentation |

---

## 10. Conclusion

Orogena represents an ambitious but achievable project to create a professional-grade terrain generation tool. The phased development approach ensures steady progress while maintaining flexibility to adapt based on testing and user feedback. The multi-scale architecture provides a unique value proposition not found in existing tools, making it suitable for serious world-builders who need geologically plausible results at every scale.

The technology stack leverages mature, well-supported libraries while allowing for future expansion. The 20-28 week timeline provides realistic milestones for a solo developer, with clear deliverables at each phase to maintain motivation and track progress.

Success depends on disciplined execution of the phased plan, early performance optimization, and continuous validation of the seamless multi-scale generation promise that differentiates Orogena from existing terrain generators.
