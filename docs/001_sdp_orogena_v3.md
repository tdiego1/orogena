# Orogena

## Complete Worldbuilding Suite

*Software Development Plan*

Version 3.0 | January 2026

---

## 1. Project Overview

### 1.1 Vision

Transform Orogena from a tectonic simulator into a **complete worldbuilding suite** following Artifexian's methodology. The application will enable fantasy authors, game developers, and world-builders to generate geologically and climatologically plausible worlds from stellar parameters down to local detail through an **immersive, cinematic interface**.

### 1.2 User Experience Philosophy

Orogena uses a **Powers of Ten** inspired interface where users experience their world at every scale—from the cosmic void of a galaxy down to the surface of their planet. This immersive journey replaces traditional menu-driven interfaces with:

- **Cinematic zoom transitions** between spatial scales
- **Full-screen 3D visualization** at each worldbuilding stage
- **Real-time visual feedback** as parameters change
- **Progressive disclosure** through presets with optional deep customization

### 1.3 Architecture: Modular Suite

Independent modules sharing a common database, enabling:
- **Incremental development**: Each module can be developed and tested in isolation
- **Incremental releases**: MVP at Phase 4, full suite later
- **Future extensibility**: New modules can be added without disrupting existing ones

### 1.4 The 19 Worldbuilding Systems

Following Artifexian's complete workflow:

| #   | System                                       | Module               |
| --- | -------------------------------------------- | -------------------- |
| 1   | Star, Planetary System, Stellar Neighborhood | `orogena_stellar`    |
| 2   | Home Planet Parameters                       | `orogena_planetary`  |
| 3   | Moon(s) & Tidal Effects                      | `orogena_planetary`  |
| 4   | Plate Tectonics & Geological History         | `orogena_global`     |
| 5   | Land Topography                              | `orogena_topography` |
| 6   | Sea Topography (Bathymetry)                  | `orogena_topography` |
| 7   | Ocean Currents                               | `orogena_ocean`      |
| 8   | Winds & Atmospheric Pressure                 | `orogena_atmosphere` |
| 9   | Upwelling & Coral Reefs                      | `orogena_ocean`      |
| 10  | Precipitation                                | `orogena_climate`    |
| 11  | Temperature & Continentality                 | `orogena_climate`    |
| 12  | Köppen Climate Classification                | `orogena_climate`    |
| 13  | Rivers & Drainage Networks                   | `orogena_hydrology`  |
| 14  | Weather Patterns                             | `orogena_weather`    |
| 15  | Rocks, Minerals, Elements                    | `orogena_geology`    |
| 16  | Fuel & Copper Age Deposits                   | `orogena_resources`  |
| 17  | Bronze Age Ore Deposits                      | `orogena_resources`  |
| 18  | Iron Age Ore Deposits                        | `orogena_resources`  |
| 19  | Salt Deposits                                | `orogena_resources`  |

### 1.5 Target Platforms

- **Primary**: Linux (CachyOS) - Development platform
- **Windows 10/11** - Primary target audience
- **macOS** (Intel and Apple Silicon) - Secondary platform

---

## 2. Technology Stack

### 2.1 Core Framework

- C++20 with Qt 6.8 for cross-platform UI and rendering
- OpenGL 4.5+ / Vulkan for GPU-accelerated simulation
- Compute shaders for parallel simulation processing

### 2.2 Key Libraries

- **Qt 6.8**: UI framework, OpenGL integration, threading (QtConcurrent)
- **GLM**: Mathematics library for 3D transformations and physics
- **FastNoise2**: High-performance noise generation for procedural detail
- **SQLite**: Unified database for all worldbuilding data
- **spdlog**: High-performance logging
- **nlohmann/json**: Project serialization and settings
- **stb_image_write**: Height map export (16-bit PNG)
- **boost::geometry**: Spatial calculations and region queries

---

## 3. System Architecture

### 3.1 Module Dependency Graph

Systems are organized into tiers. Higher tiers depend on lower tiers.

```
TIER 5 - RESOURCES
┌─────────────────────────────────────────────────────────────┐
│  Salt ← Iron Ores ← Bronze Ores ← Copper Ores ← Fuel        │
│                            ↑                                │
│                   Rocks, Minerals, Elements                 │
└─────────────────────────────────────────────────────────────┘
                             ↑
TIER 4 - HYDROLOGY & WEATHER
┌─────────────────────────────────────────────────────────────┐
│  Rivers & Drainage ← Weather Patterns ← Upwelling & Reefs   │
└─────────────────────────────────────────────────────────────┘
                             ↑
TIER 3 - CLIMATE
┌─────────────────────────────────────────────────────────────┐
│              Köppen Classification                          │
│                     ↑                                       │
│        Temperature ←→ Precipitation                         │
│              ↑              ↑                               │
│     Winds & Pressure ← Ocean Currents                       │
└─────────────────────────────────────────────────────────────┘
                             ↑
TIER 2 - TOPOGRAPHY
┌─────────────────────────────────────────────────────────────┐
│      Land Topography    ←→    Sea Topography (Bathymetry)   │
└─────────────────────────────────────────────────────────────┘
                             ↑
TIER 1 - PLANETARY FOUNDATION
┌─────────────────────────────────────────────────────────────┐
│                 Plate Tectonic Simulation                   │
│                          ↑                                  │
│                  Moon(s) & Tidal Effects                    │
│                          ↑                                  │
│                  Home Planet Parameters                     │
│                          ↑                                  │
│           Star, Planetary System, Stellar Neighborhood      │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 Library Structure

```
src/
├── core/           # orogena_core - Application foundation
├── stellar/        # orogena_stellar (NEW) - Stars, orbits, habitable zones
├── planetary/      # orogena_planetary (NEW) - Planet params, moons, tides
├── global/         # orogena_global (EXPAND) - Full tectonic simulation
├── topography/     # orogena_topography (NEW) - Land + bathymetry
├── ocean/          # orogena_ocean (NEW) - Currents, upwelling, reefs
├── atmosphere/     # orogena_atmosphere (NEW) - Winds, pressure, cells
├── climate/        # orogena_climate (NEW) - Temp, precip, Köppen
├── hydrology/      # orogena_hydrology (NEW) - Rivers, lakes, drainage
├── weather/        # orogena_weather (NEW) - Fog, storms, hurricanes
├── geology/        # orogena_geology (NEW) - Rocks, minerals, cratons
├── resources/      # orogena_resources (NEW) - Ores, fuel, salt
├── region/         # orogena_region - Regional tile system
├── local/          # orogena_local - Local detail synthesis
├── render/         # orogena_render - OpenGL visualization
├── gui/            # orogena_gui - Qt widgets interface
├── database/       # orogena_database - SQLite persistence
└── utils/          # orogena_utils - Logging and utilities
```

### 3.3 CMake Library Dependencies

```cmake
# Layer 0: Infrastructure
orogena_utils      # No dependencies

# Layer 1: Core
orogena_core       → orogena_utils

# Layer 2: Foundation Sciences
orogena_stellar    → orogena_core
orogena_planetary  → orogena_stellar
orogena_database   → orogena_core

# Layer 3: Geophysics
orogena_global     → orogena_planetary, orogena_database
orogena_topography → orogena_global

# Layer 4: Earth Systems
orogena_ocean      → orogena_topography, orogena_planetary
orogena_atmosphere → orogena_topography, orogena_planetary, orogena_ocean

# Layer 5: Climate
orogena_climate    → orogena_atmosphere, orogena_ocean
orogena_hydrology  → orogena_climate, orogena_topography
orogena_weather    → orogena_climate

# Layer 6: Geology & Resources
orogena_geology    → orogena_global, orogena_hydrology
orogena_resources  → orogena_geology, orogena_climate

# Layer 7: Multi-scale Generation
orogena_region     → orogena_topography, orogena_hydrology, orogena_geology
orogena_local      → orogena_region

# Layer 8: Presentation
orogena_render     → orogena_local, orogena_climate, orogena_resources
orogena_gui        → orogena_render

# Final executable
orogena            → orogena_gui, orogena_database
```

### 3.4 Multi-Scale Data Flow

The system operates at three spatial scales:

**Global Scale (Scale 1)**
- Input: Stellar/planetary parameters, user settings
- Process: Tectonic simulation, climate modeling
- Output: Global maps (elevation, climate, geology)
- Resolution: ~10-50km

**Regional Scale (Scale 2)**
- Input: Global data + neighbor borders
- Process: Erosion, drainage networks, resource placement
- Output: Regional tiles with full detail
- Resolution: ~1-5km
- Storage: 100×100km tiles cached in database

**Local Scale (Scale 3)**
- Input: Regional data + geology
- Process: Procedural detail synthesis
- Output: Ultra-high-res terrain
- Resolution: ~10-100m
- Storage: Generated on-demand

---

## 4. User Interface Architecture

### 4.1 Dual-Mode Interface

Orogena provides two complementary UI modes:

| Mode               | Purpose                                 | Target User                              |
| ------------------ | --------------------------------------- | ---------------------------------------- |
| **Journey Mode**   | Immersive zoom-based worldbuilding flow | New users, storytellers, guided creation |
| **Workspace Mode** | Panel-based multi-view editing          | Expert users, iterative refinement       |

Users can switch between modes at any time via `View → Switch Mode` or `Ctrl+M`.

### 4.2 Journey Mode: The Worldbuilding Experience

Journey Mode presents worldbuilding as a **cinematic narrative** through 12 immersive stages, each at a different spatial scale:

```
COSMIC SCALE (10²¹m - 10¹⁸m)
┌─────────────────────────────────────────────────────────────┐
│  Stage 1: Galaxy View                                       │
│     Dark void with spinning galaxy, select type preset      │
│     (spiral, barred spiral, elliptical, irregular)          │
│                          ↓ click to select star location    │
│  Stage 2: Stellar Neighborhood                              │
│     Nearby stars visible, optional customization            │
│                          ↓ zoom in                          │
│  Stage 3: Star Configuration                                │
│     Full star view, tweak parameters (mass, class, age)     │
│     Real-time: color/size updates as sliders change         │
└─────────────────────────────────────────────────────────────┘
                          ↓ zoom out to orbital view
STELLAR SYSTEM SCALE (10¹²m - 10⁹m)
┌─────────────────────────────────────────────────────────────┐
│  Stage 4: Orbital Configuration                             │
│     Star centered, orbital paths visible, habitable zone    │
│     in green, place planets                                 │
│                          ↓ select habitable planet          │
│  Stage 5: Planet Selection                                  │
│     Full planet view with moon(s) orbiting                  │
│     Choose presets (earth-like, super-earth, ocean world)   │
└─────────────────────────────────────────────────────────────┘
                          ↓ zoom in to planet
PLANETARY SCALE (10⁷m - 10⁴m)
┌─────────────────────────────────────────────────────────────┐
│  Stage 6: Moon Configuration                                │
│     Configure moon parameters (mass, orbit, tidal effects)  │
│                          ↓                                  │
│  Stage 7: Planet Parameters                                 │
│     Configure planet (mass, CMF, tilt, atmosphere, etc.)    │
│     Outputs: orbital period, sky color, plant color, etc.   │
└─────────────────────────────────────────────────────────────┘
                          ↓ dive to surface
SURFACE SCALE (10⁶m - 10³m)
┌─────────────────────────────────────────────────────────────┐
│  Stage 8: Tectonic Simulation                               │
│     Animate plate movement, generate heightmap              │
│     Progress overlay during simulation                      │
│                          ↓                                  │
│  Stage 9: Topography Refinement                             │
│     View/refine generated terrain, erosion settings         │
└─────────────────────────────────────────────────────────────┘
                          ↓
DETAIL SCALE (10⁵m - 10²m)
┌─────────────────────────────────────────────────────────────┐
│  Stage 10: Climate Modeling                                 │
│     Multi-layer view (temp, precip, winds, Köppen, biomes)  │
│     Ocean currents, atmospheric circulation                 │
│                          ↓                                  │
│  Stage 11: Hydrology & Resources                            │
│     Rivers, lakes, drainage networks                        │
│     Resource deposit placement                              │
│                          ↓                                  │
│  Stage 12: Surface Preview                                  │
│     Fly-through of completed world                          │
│     "World Creation Complete!" → Workspace Mode             │
└─────────────────────────────────────────────────────────────┘
```

### 4.3 Stage Transitions

Each stage follows a consistent lifecycle:

1. **ENTERING**: Cinematic zoom animation (2s ease-in-out)
2. **ACTIVE**: User interacts with parameters via slide-out panel
3. **SIMULATING**: Progress overlay for computation (if applicable)
4. **PREVIEWING**: Real-time visual feedback
5. **COMPLETING**: Save to database
6. **EXITING**: Transition to next stage

**Transition Triggers**:
- **Automatic**: Click object to select (e.g., click planet → zoom in)
- **Manual**: "Next →" button in parameter panel
- **Rewind**: Breadcrumb trail allows returning to any completed stage

### 4.4 Slide-Out Parameter Panels

Each stage has a **slide-out panel** that preserves immersion while providing full control:

```
┌────────────────────────────────────────────────────────────────────┐
│ ≡  Star Configuration          [Galaxy > Star > ...]          ?    │
├────────────────────────────────────────────────────────────────────┤
│                                                    ┌──────────────┐│
│                                                    │📌 Parameters ││
│                                                    │──────────────││
│         [IMMERSIVE 3D STAR VIEW]                   │ Presets:     ││
│                                                    │ ○ Sun-like   ││
│              ☀️                                    │ ● Red Dwarf  ││
│           (glowing star)                           │ ○ Blue Giant ││
│                                                    │              ││
│                                                    │ Mass: ████░░ ││
│                                                    │ 0.45 Msol    ││
│                                                    │              ││
│                                                    │ [Next →]     ││
│                                                    └──────────────┘│
└────────────────────────────────────────────────────────────────────┘
```

**Panel States**:

| State      | Behavior              | Trigger                            |
| ---------- | --------------------- | ---------------------------------- |
| **HIDDEN** | Only edge tab visible | Mouse leaves for 3s (if auto-hide) |
| **PEEK**   | Temporarily visible   | Hover over edge tab                |
| **PINNED** | Permanently visible   | Click 📌 pin icon or `Ctrl+Tab`     |

**Pin Behavior**:
- Click 📌 to toggle between auto-hide and pinned
- Pin state saved per-stage in user preferences
- Global default settable: `Settings → Journey → Default Panel Behavior`
- Keyboard shortcut: `Tab` toggles visibility, `Ctrl+Tab` toggles pin

### 4.5 Real-Time Visual Feedback

Every parameter change **immediately updates the 3D view**:

| Parameter              | Visual Feedback                                      |
| ---------------------- | ---------------------------------------------------- |
| Star mass slider       | Star size scales, color shifts (red → yellow → blue) |
| Axial tilt slider      | Planet rotation axis tilts visibly                   |
| Plate count slider     | Voronoi cells regenerate (debounced 500ms)           |
| Moon orbit radius      | Orbit path animates in real-time                     |
| Climate layer toggle   | Overlay crossfades (temp ↔ precip ↔ biomes)          |
| Atmosphere composition | Sky color updates, plant color recalculates          |

### 4.6 Workspace Mode: Expert Editing

After completing Journey Mode (or for returning projects), users access **Workspace Mode**:

```
┌─────────────────────────────────────────────────────────────────────────┐
│  Menu Bar: File | Edit | View | Simulation | Export | Window | Help     │
├─────────────────────────────────────────────────────────────────────────┤
│  Toolbar: [New] [Open] [Save] │ [Global] [Regional] [Local] │ [Play]    │
│           Workspace:  [▼ Climate Modeling ▼]                            │
├───────┬─────────────────────────────────────────────────────┬───────────┤
│       │                                                     │           │
│ Scene │              PRIMARY VIEWPORT                       │ Layer     │
│ Tree  │           (3D Globe / 2D Map View)                  │ Stack     │
│       │                                                     │           │
│ [+]   │                                                     │ ☑ Plates  │
│ World │                    🌍                               │ ☑ Topo    │
│  Star │                                                     │ ☐ Climate │
│  Plan │                                                     │ ☐ Rivers  │
│  Tect │                                                     │           │
│       │  Timeline: ─────●──────────────────────────         │ Opacity:  │
│       │           0 Ma        250 Ma           Present      │ ████ 80%  │
├───────┴──────────────────┬──────────────────────────────────┴───────────┤
│ Parameters (Tabbed)      │  Properties / Info                           │
│ ┌──────────────────────┐ │ ┌────────────────────────────────────────┐   │
│ │[General][Climate]    │ │ │ Selected: Tropical Rainforest (Af)     │   │
│ │ Greenhouse: ████░░   │ │ │ Location: 5°N 78°W                     │   │
│ │ Ocean Heat: ███░░░   │ │ │ Temp: 24-28°C | Precip: 3200mm/yr      │   │
│ └──────────────────────┘ │ └────────────────────────────────────────┘   │
├──────────────────────────┴──────────────────────────────────────────────┤
│ Status: Ready │ Scale: Global (50km/px) │ GPU: RTX 4090 │ FPS: 60       │
└─────────────────────────────────────────────────────────────────────────┘
```

**Workspace Mode Features**:
- **Dockable panels**: Rearrange, float, tab panels freely
- **Multiple viewports**: Split view, compare January vs July
- **Layer stack**: Toggle any data layer with opacity control
- **Scene tree**: Navigate all 19 systems hierarchically
- **Timeline scrubber**: Geological history playback
- **Predefined workspaces**: Stellar, Tectonics, Climate, Resources, Export

### 4.7 Navigation System

Three navigation mechanisms work together:

1. **Linear Flow** (Journey Mode): Stages 1→12 sequentially for first-time users
2. **Breadcrumb Trail**: Timeline at top shows progress, click to revisit any stage
3. **Scene Tree** (Workspace Mode): Hierarchical access to all systems

**Breadcrumb Trail**:
```
┌────────────────────────────────────────────────────────────────┐
│ Galaxy > Star > Orbit > Planet > Moons > Tectonics > Climate   │
│   ✓      ✓       ✓       ✓        •(now)    ○         ○        │
└────────────────────────────────────────────────────────────────┘
```

**Scene Tree** (Workspace Mode):
```
🌟 MyWorld
 ├─ ⭐ Stellar System         [✓ CONFIGURED]
 │   ├─ Star: G2V (1.05 M☉)
 │   └─ Habitable Zone: 0.95-1.4 AU
 ├─ 🌍 Home Planet            [✓ CONFIGURED]
 │   ├─ Mass: 1.2 M⊕
 │   └─ Rotation: 26.4 hrs
 ├─ 🌙 Moons (1)              [✓ CONFIGURED]
 ├─ 🌋 Plate Tectonics        [IN PROGRESS ⚠]
 ├─ ⛰️ Topography             [🔒 LOCKED]
 ├─ 🌊 Ocean Currents         [🔒 LOCKED]
 ├─ 🌬️ Atmosphere             [🔒 LOCKED]
 ├─ 🌡️ Climate                [🔒 LOCKED]
 ├─ 💧 Hydrology              [🔒 LOCKED]
 ├─ ⛈️ Weather                [🔒 LOCKED]
 ├─ 🪨 Geology                [🔒 LOCKED]
 └─ ⚒️ Resources              [🔒 LOCKED]
```

### 4.8 Camera System

The rendering system uses a **multi-scale zoom camera** with logarithmic distance transitions:

```cpp
enum class CameraMode
{
    GALAXY,      // 10²¹m - galaxy view, orthographic
    STELLAR,     // 10¹²m - stellar neighborhood
    ORBITAL,     // 10⁹m  - planetary orbits visible
    PLANETARY,   // 10⁷m  - full planet view
    SURFACE,     // 10⁴m  - terrain visible
    REGIONAL,    // 10³m  - regional tiles
    LOCAL        // 10²m  - ultra-detail
};
```

**Transition Animation**:
- **Duration**: 2 seconds (configurable)
- **Easing**: Cubic ease-in-out for cinematic feel
- **Scale interpolation**: Logarithmic (smooth power-of-ten jumps)
- **Object tracking**: Camera follows selected object during transition

### 4.9 Stage-Specific Renderers

Each Journey stage uses specialized rendering:

| Stage                | Renderer            | Technique                                    |
| -------------------- | ------------------- | -------------------------------------------- |
| Galaxy               | `GalaxyRenderer`    | GPU particle system (100K+ stars)            |
| Stellar Neighborhood | `StarFieldRenderer` | HDR bloom, sprite sizing by luminosity       |
| Star Config          | `StarRenderer`      | Procedural surface shader, corona layers     |
| Orbital Config       | `OrbitRenderer`     | Ellipse paths, habitable zone overlay        |
| Planet/Moon          | `PlanetRenderer`    | Icosphere + atmosphere rim shader            |
| Tectonics            | `TectonicRenderer`  | Voronoi plates, velocity arrows              |
| Topography           | `TerrainRenderer`   | Heightmap displacement, hypsometric tinting  |
| Climate              | `ClimateRenderer`   | Multi-layer texture overlay, wind flow lines |
| Hydrology            | `HydrologyRenderer` | River network lines, resource icons          |
| Surface              | `DetailRenderer`    | Procedural detail, LOD tessellation          |

### 4.10 UI Module Structure

New files for the UI system:

```
src/gui/
├── gui_main_window.h/cpp           # Mode switching (Journey ↔ Workspace)
├── journey/                         # Journey Mode components
│   ├── gui_journey_controller.h/cpp # Stage orchestration
│   ├── gui_stage_base.h/cpp         # Shared stage template
│   ├── gui_stage_galaxy.h/cpp       # Stage 1
│   ├── gui_stage_stellar.h/cpp      # Stage 2-3
│   ├── gui_stage_orbital.h/cpp      # Stage 4
│   ├── gui_stage_planet.h/cpp       # Stage 5-7
│   ├── gui_stage_tectonics.h/cpp    # Stage 8
│   ├── gui_stage_topography.h/cpp   # Stage 9
│   ├── gui_stage_climate.h/cpp      # Stage 10
│   ├── gui_stage_hydrology.h/cpp    # Stage 11
│   └── gui_stage_preview.h/cpp      # Stage 12
├── workspace/                       # Workspace Mode components
│   ├── gui_workspace_controller.h/cpp
│   ├── gui_scene_tree.h/cpp
│   ├── gui_layer_stack.h/cpp
│   ├── gui_properties_panel.h/cpp
│   └── gui_timeline.h/cpp
├── widgets/                         # Shared custom widgets
│   ├── gui_slide_panel.h/cpp        # Auto-hide/pinnable panel
│   ├── gui_breadcrumb.h/cpp         # Navigation breadcrumb
│   ├── gui_parameter_group.h/cpp    # Collapsible parameter section
│   └── gui_smart_slider.h/cpp       # Real-time preview slider
└── render/                          # Extended for Journey Mode
    ├── render_zoom_camera.h/cpp     # Multi-scale camera
    ├── render_galaxy.h/cpp          # Galaxy particle system
    └── render_transition.h/cpp      # Zoom transition animations
```

---

## 5. Database Schema

SQLite database manages all worldbuilding data:

### 5.1 Core Tables

```sql
-- Stellar Systems
CREATE TABLE stellar_systems (
    system_id TEXT PRIMARY KEY,
    star_mass REAL NOT NULL,           -- Solar masses
    star_luminosity REAL NOT NULL,     -- Solar luminosities
    spectral_class TEXT NOT NULL,      -- O, B, A, F, G, K, M
    star_age_gyr REAL,                 -- Billions of years
    hz_inner_au REAL,                  -- Habitable zone inner edge
    hz_outer_au REAL,                  -- Habitable zone outer edge
    metadata JSON,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Planets
CREATE TABLE planets (
    planet_id TEXT PRIMARY KEY,
    system_id TEXT REFERENCES stellar_systems(system_id),
    name TEXT,
    seed INTEGER NOT NULL,
    mass_earth REAL NOT NULL,          -- Earth masses
    radius_earth REAL NOT NULL,        -- Earth radii
    rotation_hours REAL NOT NULL,      -- Day length
    axial_tilt_deg REAL NOT NULL,      -- Seasons
    orbital_radius_au REAL NOT NULL,   -- Distance from star
    orbital_period_days REAL,          -- Year length
    resolution_km INTEGER,             -- Global grid resolution
    tectonic_history BLOB,
    global_uplift_map BLOB,
    plate_configuration BLOB,
    metadata JSON,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Moons
CREATE TABLE moons (
    moon_id TEXT PRIMARY KEY,
    planet_id TEXT REFERENCES planets(planet_id),
    name TEXT,
    mass_kg REAL NOT NULL,
    orbital_radius_km REAL NOT NULL,
    orbital_period_hours REAL NOT NULL,
    tidal_force_relative REAL,         -- Relative to Earth's moon
    metadata JSON
);

-- Geological Events Timeline
CREATE TABLE geological_events (
    event_id TEXT PRIMARY KEY,
    planet_id TEXT REFERENCES planets(planet_id),
    event_type TEXT NOT NULL,          -- OROGENY, SUBDUCTION, LIP, HOTSPOT, etc.
    age_mya REAL NOT NULL,             -- Millions of years ago
    duration_my REAL,
    location_lat REAL,
    location_lon REAL,
    magnitude REAL,
    details JSON
);

-- Climate Data (global scale)
CREATE TABLE climate_global (
    climate_id TEXT PRIMARY KEY,
    planet_id TEXT REFERENCES planets(planet_id),
    resolution_km INTEGER NOT NULL,
    temperature_january BLOB,
    temperature_july BLOB,
    precipitation_annual BLOB,
    koppen_zones BLOB,
    biome_map BLOB,                 -- Biome classification
    wind_field_jan BLOB,
    wind_field_jul BLOB,
    pressure_field_jan BLOB,
    pressure_field_jul BLOB,
    generated_at TIMESTAMP
);

-- Ocean Data
CREATE TABLE ocean_data (
    ocean_id TEXT PRIMARY KEY,
    planet_id TEXT REFERENCES planets(planet_id),
    surface_currents BLOB,
    deep_currents BLOB,
    upwelling_zones BLOB,
    coral_reef_zones BLOB,
    generated_at TIMESTAMP
);

-- Geology Data (global scale)
CREATE TABLE geology_global (
    geology_id TEXT PRIMARY KEY,
    planet_id TEXT REFERENCES planets(planet_id),
    rock_types_map BLOB,
    craton_boundaries BLOB,
    mineral_concentrations BLOB,
    generated_at TIMESTAMP
);

-- Resource Deposits
CREATE TABLE resource_deposits (
    deposit_id TEXT PRIMARY KEY,
    planet_id TEXT REFERENCES planets(planet_id),
    resource_type TEXT NOT NULL,       -- oil, gas, coal, copper, tin, iron, salt
    location_lat REAL NOT NULL,
    location_lon REAL NOT NULL,
    extent_km2 REAL,
    abundance TEXT,                    -- scarce, moderate, rich
    accessibility TEXT,                -- surface, shallow, deep
    discovery_era TEXT,                -- copper_age, bronze_age, iron_age, modern
    metadata JSON
);

-- Regional Storage (cached tiles)
CREATE TABLE storage_regions (
    region_id TEXT PRIMARY KEY,
    planet_id TEXT REFERENCES planets(planet_id),
    grid_x INTEGER NOT NULL,
    grid_y INTEGER NOT NULL,
    size_km INTEGER NOT NULL,
    resolution_m INTEGER NOT NULL,
    heightmap BLOB,
    river_network BLOB,
    geology_data BLOB,
    border_data BLOB,
    climate_data BLOB,
    resource_data BLOB,
    generated_at TIMESTAMP,
    UNIQUE(planet_id, grid_x, grid_y)
);

-- Indexes
CREATE INDEX idx_planets_system ON planets(system_id);
CREATE INDEX idx_moons_planet ON moons(planet_id);
CREATE INDEX idx_events_planet ON geological_events(planet_id);
CREATE INDEX idx_events_age ON geological_events(age_mya);
CREATE INDEX idx_deposits_planet ON resource_deposits(planet_id);
CREATE INDEX idx_deposits_type ON resource_deposits(resource_type);
CREATE INDEX idx_region_lookup ON storage_regions(planet_id, grid_x, grid_y);
```

### 5.2 Journey Progress Tables

```sql
-- Journey progress tracking (for Journey Mode)
CREATE TABLE worldbuilding_journey (
    planet_id TEXT PRIMARY KEY REFERENCES planets(planet_id),
    current_stage INTEGER NOT NULL DEFAULT 0,
    ui_mode TEXT NOT NULL DEFAULT 'journey',  -- 'journey' or 'workspace'
    created_at INTEGER NOT NULL,
    updated_at INTEGER NOT NULL
);

-- Stage-specific parameters saved during Journey
CREATE TABLE journey_stage_params (
    planet_id TEXT NOT NULL REFERENCES planets(planet_id),
    stage INTEGER NOT NULL,
    parameter_key TEXT NOT NULL,
    parameter_value TEXT NOT NULL,
    PRIMARY KEY (planet_id, stage, parameter_key)
);

-- Panel preferences per stage
CREATE TABLE journey_panel_prefs (
    planet_id TEXT NOT NULL REFERENCES planets(planet_id),
    stage INTEGER NOT NULL,
    panel_id TEXT NOT NULL,
    is_pinned BOOLEAN NOT NULL DEFAULT 0,
    position_x INTEGER,
    position_y INTEGER,
    width INTEGER,
    height INTEGER,
    PRIMARY KEY (planet_id, stage, panel_id)
);

CREATE INDEX idx_journey_stage ON journey_stage_params(planet_id, stage);
```

---

## 6. Development Phases

### Phase 0: Foundation Completion (Weeks 1-3)

**Goal**: Complete current Phase 1 infrastructure before expanding scope.

**Tasks**:
- Implement SQLite database wrapper with connection pooling
- Complete settings system using QSettings
- Create project save/load infrastructure with JSON serialization
- Finalize OpenGL viewport setup

**Key Files**:
- `src/database/database_manager.cpp/.h`
- `src/core/core_settings.cpp/.h`
- `src/core/core_project.cpp/.h`

**Deliverable**: Running application with database, settings, and save/load functionality.

---

### Phase 1: Stellar Foundation (Weeks 4-6)

**Goal**: Implement stellar system simulation with habitable zone calculations.

**New Module**: `orogena_stellar`

**Features**:
- Star parameters: mass, luminosity, spectral class, age
- Habitable zone calculation (inner/outer bounds)
- Planetary orbital mechanics (Kepler's laws)
- Day length, year length from orbital parameters
- Parameter validation with warnings for unphysical values

**Rendering Updates** (`orogena_render`):
- Basic sphere rendering with OpenGL
- Camera system (arcball rotation, zoom)
- Simple lighting (directional light)
- Wireframe/solid toggle

**Key Files**:
- `src/stellar/stellar_star.cpp/.h`
- `src/stellar/stellar_system.cpp/.h`
- `src/stellar/stellar_habitable_zone.cpp/.h`
- `src/render/render_sphere.cpp/.h`
- `src/render/render_camera.cpp/.h`
- `src/render/render_shader.cpp/.h`

**Deliverable**: Stellar system configuration with validated parameters + basic 3D sphere visualization.

---

### Phase 2: Planetary Parameters (Weeks 7-9)

**Goal**: Define home planet and moon characteristics.

**New Module**: `orogena_planetary`

**Features**:
- Planet mass, radius, density, surface gravity
- Rotation period (day length), axial tilt
- Moon(s): mass, orbital radius, period
- Tidal force calculations from moons + star
- Seasonal variation from axial tilt

**Rendering Updates** (`orogena_render`):
- Icosphere mesh generation (subdivision-based)
- Texture coordinate mapping (UV sphere)
- Basic color mapping (water = blue, land = green placeholder)
- Orbit visualization for moon(s)

**Key Files**:
- `src/planetary/planetary_parameters.cpp/.h`
- `src/planetary/planetary_moon.cpp/.h`
- `src/planetary/planetary_tides.cpp/.h`
- `src/planetary/planetary_seasons.cpp/.h`
- `src/render/render_icosphere.cpp/.h`
- `src/render/render_mesh.cpp/.h`

**Deliverable**: Complete planet configuration with calculated tides and seasons + textured sphere rendering.

---

### Phase 3: Enhanced Plate Tectonics (Weeks 10-16)

**Goal**: Full geological simulation following Artifexian's methodology.

**Expand Module**: `orogena_global`

**Features** (weekly breakdown):
- **Week 10-11**: Supercontinent cycles (Pangea formation/breakup modeling)
- **Week 11-12**: Subduction mechanics (oceanic-continental collision, trench formation)
- **Week 12**: Microcontinents (terranes, island arcs, accretion)
- **Week 13**: Orogeny (multi-phase mountain building)
- **Week 14**: Large Igneous Provinces (LIP events, flood basalts)
- **Week 14-15**: Hotspot volcanism (mantle plumes, island chains)
- **Week 15-16**: Full geological history timeline with events

**Rendering Updates** (`orogena_render`):
- Plate boundary visualization (colored lines/edges)
- Velocity vector field rendering (arrows on sphere)
- Animation system for plate motion playback
- Time slider for geological history scrubbing
- Color-coded plates (continental vs oceanic)

**Key Files**:
- `src/global/global_supercontinent.cpp/.h`
- `src/global/global_subduction.cpp/.h`
- `src/global/global_orogeny.cpp/.h`
- `src/global/global_hotspot.cpp/.h`
- `src/global/global_lip.cpp/.h`
- `src/global/global_geology_history.cpp/.h`
- `src/render/render_plate_boundaries.cpp/.h`
- `src/render/render_vector_field.cpp/.h`
- `src/render/render_animation.cpp/.h`

**Deliverable**: Complete tectonic simulation with geologically plausible features + animated plate visualization.

---

### Phase 4: Topography Generation (Weeks 17-20) — **MVP RELEASE v1.0**

**Goal**: High-fidelity land and sea floor topography with full 3D visualization.

**New Module**: `orogena_topography`

**Features**:
- **Land topography** (Week 17-18):
  - Mountain ranges from orogeny data
  - Plateau formation
  - Rift valleys
  - Volcanic edifices

- **Bathymetry** (Week 19-20):
  - Mid-ocean ridges
  - Abyssal plains
  - Oceanic trenches
  - Seamounts and guyots
  - Continental shelves

**Rendering Updates** (`orogena_render`):
- Heightmap-based terrain displacement on sphere
- Normal map generation from heightmap for lighting
- Hypsometric tinting (elevation-based color gradients)
- Level-of-detail (LOD) system for adaptive mesh resolution
- Shaded relief rendering (hillshade + color)
- Water plane rendering with transparency
- Export framebuffer to PNG (screenshots)

**Key Files**:
- `src/topography/topo_land_generator.cpp/.h`
- `src/topography/topo_bathymetry.cpp/.h`
- `src/topography/topo_continental_shelf.cpp/.h`
- `src/render/render_terrain.cpp/.h`
- `src/render/render_heightmap.cpp/.h`
- `src/render/render_lod.cpp/.h`
- `src/render/render_hypsometric.cpp/.h`
- `src/render/render_export.cpp/.h`

**Deliverable**: Complete terrain generation pipeline with full 3D visualization. **MVP Release v1.0**.

---

### Phase 5: Ocean Currents (Weeks 21-23)

**Goal**: Simulate global ocean circulation.

**New Module**: `orogena_ocean`

**Features**:
- Surface currents driven by wind
- Thermohaline circulation (deep currents)
- Major gyres (subtropical, subpolar)
- Boundary currents (western intensification)
- Upwelling zones
- Coral reef distribution based on temperature/currents

**Key Files**:
- `src/ocean/ocean_current_sim.cpp/.h`
- `src/ocean/ocean_gyre.cpp/.h`
- `src/ocean/ocean_upwelling.cpp/.h`
- `src/ocean/ocean_coral_reef.cpp/.h`

**Deliverable**: Realistic ocean circulation model.

---

### Phase 6: Atmospheric Circulation (Weeks 24-26)

**Goal**: Wind patterns and pressure systems.

**New Module**: `orogena_atmosphere`

**Features**:
- Hadley, Ferrel, Polar cells
- Trade winds, westerlies, polar easterlies
- Pressure cells (subtropical highs, subpolar lows)
- Monsoon systems
- Orographic effects (rain shadows)
- Jet streams

**Key Files**:
- `src/atmosphere/atmo_circulation.cpp/.h`
- `src/atmosphere/atmo_wind.cpp/.h`
- `src/atmosphere/atmo_pressure.cpp/.h`
- `src/atmosphere/atmo_monsoon.cpp/.h`

**Deliverable**: Global atmospheric circulation model.

---

### Phase 7: Climate System (Weeks 27-32)

**Goal**: Temperature, precipitation, Köppen classification, and biome mapping.

**New Module**: `orogena_climate`

**Features**:
- **Temperature** (Week 27-28):
  - Solar input by latitude
  - Continentality effects
  - Ocean current modification
  - Altitude lapse rate
  - Isotherms

- **Precipitation** (Week 29-30):
  - Orographic precipitation
  - Convective precipitation
  - Frontal precipitation
  - Seasonal variation

- **Köppen Classification** (Week 31):
  - Full Köppen-Geiger system
  - Climate zone boundaries

- **Biome Mapping** (Week 32):
  - Biome derivation from Köppen zones
  - Elevation variants (alpine, montane, lowland)
  - Water proximity effects (riparian, coastal, mangrove)
  - Seasonal extreme refinements
  - Biome types: tropical rainforest, savanna, desert, steppe, temperate forest, boreal/taiga, tundra, alpine, wetland, etc.

**Key Files**:
- `src/climate/climate_temperature.cpp/.h`
- `src/climate/climate_precipitation.cpp/.h`
- `src/climate/climate_koppen.cpp/.h`
- `src/climate/climate_continentality.cpp/.h`
- `src/climate/climate_biome.cpp/.h`

**Deliverable**: Complete climate system with Köppen zones and biome mapping.

---

### Phase 8: Hydrology (Weeks 33-36)

**Goal**: Rivers, lakes, and drainage networks.

**New Module**: `orogena_hydrology`

**Features**:
- Drainage basin delineation
- River network generation (stream ordering)
- Lake formation (tectonic, glacial, volcanic)
- Watershed boundaries
- Endorheic basins
- River discharge estimation

**Key Files**:
- `src/hydrology/hydro_drainage.cpp/.h`
- `src/hydrology/hydro_river.cpp/.h`
- `src/hydrology/hydro_lake.cpp/.h`
- `src/hydrology/hydro_watershed.cpp/.h`

**Deliverable**: Realistic river and lake networks.

---

### Phase 9: Weather Patterns (Weeks 37-39)

**Goal**: Regional weather phenomena.

**New Module**: `orogena_weather`

**Features**:
- Fog zones (coastal, radiation, advection)
- Thunderstorm frequency maps
- Tornado alleys
- Hurricane/typhoon/cyclone tracks
- Blizzard zones
- Dust storm regions
- Aurora zones (polar)

**Key Files**:
- `src/weather/weather_fog.cpp/.h`
- `src/weather/weather_thunderstorm.cpp/.h`
- `src/weather/weather_tornado.cpp/.h`
- `src/weather/weather_hurricane.cpp/.h`
- `src/weather/weather_blizzard.cpp/.h`
- `src/weather/weather_dust_storm.cpp/.h`
- `src/weather/weather_aurora.cpp/.h`

**Deliverable**: Weather pattern distribution maps.

---

### Phase 10: Geology & Minerals (Weeks 40-44)

**Goal**: Rock types and mineral distributions.

**New Module**: `orogena_geology`

**Features**:
- **Rock types** (Week 40-41):
  - Igneous: volcanic vs. plutonic, composition
  - Sedimentary: clastic, chemical, organic
  - Metamorphic: contact vs. regional

- **Mineral distributions** (Week 42-43):
  - Based on rock type + geological history
  - Element concentrations

- **Archean cratons** (Week 44):
  - Ancient stable continental cores
  - Shield regions

**Key Files**:
- `src/geology/geo_rock_type.cpp/.h`
- `src/geology/geo_mineral.cpp/.h`
- `src/geology/geo_element.cpp/.h`
- `src/geology/geo_craton.cpp/.h`

**Deliverable**: Complete rock and mineral distribution model.

---

### Phase 11: Resource Deposits (Weeks 45-50)

**Goal**: Economic resources following historical availability.

**New Module**: `orogena_resources`

**Features**:
- **Fuel deposits** (Week 45-46):
  - Oil: source rocks + traps + maturation
  - Natural gas: associated with oil or coal
  - Coal: ancient swamps

- **Copper Age ores** (Week 47):
  - Native copper
  - Malachite, azurite (surface oxidation)

- **Bronze Age ores** (Week 48):
  - Tin: cassiterite in granites
  - Arsenic bronzes

- **Iron Age ores** (Week 49):
  - Bog iron
  - Hematite, magnetite
  - Banded iron formations

- **Salt deposits** (Week 50):
  - Evaporite basins
  - Salt domes
  - Sea salt potential

**Key Files**:
- `src/resources/res_fuel.cpp/.h`
- `src/resources/res_ore_copper.cpp/.h`
- `src/resources/res_ore_bronze.cpp/.h`
- `src/resources/res_ore_iron.cpp/.h`
- `src/resources/res_salt.cpp/.h`

**Deliverable**: Complete resource distribution model.

---

### Phase 12: Integration & Polish (Weeks 51-56) — **FULL RELEASE v2.0**

**Goal**: UI integration, export, documentation.

**Features**:
- Unified world creation wizard
- Layer visualization system (toggle any data layer)
- Export formats (GeoTIFF, SVG maps, JSON)
- Performance optimization
- Batch processing for large worlds
- Documentation and tutorials

**Deliverable**: Complete worldbuilding suite. **Full Release v2.0**.

---

## 7. MVP Scope (v1.0 at Week 20)

### Included in MVP

**Simulation & Data**:
- Stellar system configuration (star type, habitable zone)
- Planet parameters (size, rotation, tilt, moons, tides)
- Full plate tectonic simulation:
  - Supercontinent cycles
  - Subduction and island arcs
  - Microcontinents
  - Orogeny
  - Large Igneous Provinces
  - Hotspots
  - Complete geological history
- Land and sea topography generation
- Multi-scale terrain export

**User Interface (Journey Mode - Stages 1-9)**:
- Galaxy view with type selection (spiral, elliptical, irregular)
- Stellar neighborhood visualization
- Star configuration with real-time visual feedback
- Orbital configuration with habitable zone display
- Planet/moon parameter configuration
- Tectonic simulation with animated plate movement
- Topography generation and preview
- Pinnable slide-out parameter panels
- Breadcrumb navigation between stages
- Smooth cinematic zoom transitions

**3D Visualization**:
- Interactive 3D globe rendering with OpenGL
- Multi-scale zoom camera (galaxy → planet → surface)
- Heightmap-based terrain displacement
- Hypsometric tinting (elevation colors)
- Plate boundary visualization
- Animated plate motion playback
- Shaded relief rendering (hillshade lighting)
- Level-of-detail (LOD) mesh optimization
- Screenshot export to PNG

### Deferred to v2.0

- Journey Mode Stages 10-12 (Climate, Hydrology, Surface Preview)
- Workspace Mode (full panel-based editing)
- Ocean currents, atmosphere, climate (Phases 5-7)
- Rivers, weather patterns (Phases 8-9)
- Geology, minerals, resources (Phases 10-11)
- Advanced rendering (regional/local scale detail, GPU compute shaders for procedural generation)

### MVP Value Proposition

Complete terrain generation from stellar parameters down to regional detail with full interactive 3D visualization — the core Artifexian workflow for physical geography in a usable, visually compelling application.

---

## 8. Quality Assurance & Testing

### 8.1 Testing Strategy

- Unit tests for core algorithms (orbital mechanics, plate physics, erosion)
- Integration tests for multi-module pipelines
- Visual regression tests for rendering output
- Performance benchmarks for all scales
- Cross-platform testing on Linux, Windows, macOS

### 8.2 Validation Criteria

| System           | Validation                                                       |
| ---------------- | ---------------------------------------------------------------- |
| Stellar          | Habitable zone matches known exoplanet systems                   |
| Planetary        | Day/night, seasons match Earth-like calculations                 |
| Tectonics        | Realistic continent shapes, no artifacts                         |
| Topography       | Mountains/ocean floors match real-world patterns                 |
| **Rendering**    | **60 FPS at 1080p, no visual artifacts, smooth camera controls** |
| **Journey Mode** | **Smooth transitions (<2s), panels responsive, no stutter**      |
| **Zoom Camera**  | **Logarithmic scale transitions feel natural and cinematic**     |
| Ocean            | Currents resemble Earth's major gyres                            |
| Atmosphere       | Wind patterns produce correct rain shadows                       |
| Climate          | Köppen zones match test cases                                    |
| Biomes           | Biome distribution matches Köppen + elevation expectations       |
| Hydrology        | Rivers flow realistically to ocean/lakes                         |
| Weather          | Patterns in expected locations                                   |
| Geology          | Rock distribution geologically plausible                         |
| Resources        | Deposits in appropriate geological contexts                      |

### 8.3 Performance Targets

| Operation                    | Target                      |
| ---------------------------- | --------------------------- |
| Global simulation step       | <5s @ 2048² resolution      |
| Regional tile generation     | <30s @ 100×100km            |
| Local detail synthesis       | <1s                         |
| Rendering (global view)      | 60 FPS                      |
| Stage transition animation   | <2s with 60 FPS during zoom |
| Panel slide animation        | <300ms                      |
| Real-time parameter feedback | <100ms latency              |
| Memory usage                 | <2GB for typical planet     |

---

## 9. Risk Mitigation

### 9.1 Technical Risks

| Risk                              | Severity | Mitigation                                    |
| --------------------------------- | -------- | --------------------------------------------- |
| Scope creep from 19 systems       | High     | Strict phase boundaries, MVP per module       |
| Performance bottlenecks           | High     | GPU compute, caching, LOD                     |
| Cross-module data consistency     | High     | Single database, transaction management       |
| Complex UI for all systems        | Medium   | Progressive disclosure, sensible defaults     |
| Scientific accuracy vs. usability | Medium   | "Plausible" not "perfect", adjustable realism |
| GPU shader compatibility          | Medium   | CPU fallbacks, shader variants                |

### 9.2 Project Risks

| Risk                      | Severity | Mitigation                                 |
| ------------------------- | -------- | ------------------------------------------ |
| Solo developer burnout    | High     | Incremental releases, usable milestones    |
| Feature interdependencies | High     | Bottom-up development per dependency graph |
| User learning curve       | Medium   | Tutorials, presets, wizard workflow        |
| Testing coverage gaps     | Medium   | Test pyramid, automated CI                 |

---

## 10. Timeline Summary

| Phase  | Weeks     | Cumulative  | Deliverable                    |
| ------ | --------- | ----------- | ------------------------------ |
| 0      | 1-3       | Week 3      | Foundation complete            |
| 1      | 4-6       | Week 6      | Stellar system                 |
| 2      | 7-9       | Week 9      | Planet + moons                 |
| 3      | 10-16     | Week 16     | Full tectonics                 |
| **4**  | **17-20** | **Week 20** | **Topography — MVP v1.0**      |
| 5      | 21-23     | Week 23     | Ocean currents                 |
| 6      | 24-26     | Week 26     | Atmosphere                     |
| 7      | 27-32     | Week 32     | Climate + Köppen               |
| 8      | 33-36     | Week 36     | Rivers                         |
| 9      | 37-39     | Week 39     | Weather                        |
| 10     | 40-44     | Week 44     | Geology                        |
| 11     | 45-50     | Week 50     | Resources                      |
| **12** | **51-56** | **Week 56** | **Polish — Full Release v2.0** |

**MVP (v1.0)**: Week 20 — Star → Planet → Tectonics → Topography
**Full Suite (v2.0)**: Week 56 (~13 months total)

---

## 11. Conclusion

This expanded plan transforms Orogena into a comprehensive worldbuilding suite covering all 19 systems in Artifexian's methodology. The **immersive Journey Mode** provides a cinematic, zoom-based experience from galaxy to planet surface, while **Workspace Mode** offers expert users full control through a panel-based interface.

Key success factors:
1. **Immersive experience**: Journey Mode guides users through worldbuilding as a narrative
2. **Strict dependency order**: Build systems bottom-up per the dependency graph
3. **Incremental value**: Each phase produces usable features
4. **Modular isolation**: Each library testable independently
5. **Reasonable defaults**: Earth-like presets for quick starts
6. **Performance discipline**: Profile early, optimize continuously
7. **Dual-mode flexibility**: Journey Mode for creation, Workspace Mode for refinement

The 56-week timeline is ambitious but achievable for a focused solo developer, with the Week 20 MVP providing an early deliverable to maintain momentum and gather feedback.

---

**Document Version**: 3.0 (January 2026)
**Last Updated**: Added immersive Journey Mode UI architecture with 12 zoom-based stages, pinnable slide-out panels, and dual-mode (Journey/Workspace) interface design
**Supersedes**: Version 2.1 (January 2026)
