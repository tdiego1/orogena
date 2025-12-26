# Core library (shared by all components)
add_library(orogena_core
    core/Application.cpp
    core/Project.cpp
    core/Settings.cpp
)

target_link_libraries(orogena_core
    PUBLIC
        Qt6::Core
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)

# Global simulation library (Scale 1)
add_library(orogena_global
    global/TectonicSimulation.cpp
    global/PlateManager.cpp
    global/Plate.cpp
    global/GlobalHeightMap.cpp
)

target_link_libraries(orogena_global
    PUBLIC
        orogena_core
        glm::glm
)

# Regional generation library (Scale 2)
add_library(orogena_regional
    regional/RegionManager.cpp
    regional/StreamPowerErosion.cpp
    regional/BorderStitcher.cpp
)

target_link_libraries(orogena_regional
    PUBLIC
        orogena_core
        orogena_global
)

# Local detail library (Scale 3)
add_library(orogena_local
    local/DetailSynthesizer.cpp
    local/GeologyAwareNoise.cpp
)

target_link_libraries(orogena_local
    PUBLIC
        orogena_regional
)

# Rendering library
add_library(orogena_rendering
    rendering/MultiScaleRenderer.cpp
    rendering/HeightMapRenderer.cpp
    rendering/Camera.cpp
)

target_link_libraries(orogena_rendering
    PUBLIC
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        OpenGL::GL
        orogena_local
)

# UI library
add_library(orogena_ui
    ui/MainWindow.cpp
    ui/GlobalViewport.cpp
)

target_link_libraries(orogena_ui
    PUBLIC
        Qt6::Widgets
        orogena_rendering
)

# Database library
add_library(orogena_database
    database/DatabaseManager.cpp
)

target_link_libraries(orogena_database
    PUBLIC
        Qt6::Sql
        orogena_core
)

# Utilities library
add_library(orogena_utils
    utils/utils_logger.cpp
    utils/utils_types.h
)

target_link_libraries(orogena_utils
    PUBLIC
        spdlog::spdlog
)

# Main executable
add_executable(orogena
    main.cpp
)

target_link_libraries(orogena
    PRIVATE
        orogena_ui
        orogena_database
        orogena_utils
)

# Set executable properties
set_target_properties(orogena PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
