# Core library (shared by all components)
add_library(orogena_core
    core/core_application.cpp
    core/core_project.cpp
    core/core_settings.cpp
)

target_link_libraries(orogena_core
    PUBLIC
        Qt6::Core
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)

# Global simulation library (Scale 1)
add_library(orogena_global
    global/global_tectonic_sim.cpp
    global/global_plate_manager.cpp
    global/global_plate.cpp
    global/global_heightmap.cpp
)

target_link_libraries(orogena_global
    PUBLIC
        orogena_core
        glm::glm
)

# Region generation library (Scale 2)
add_library(orogena_region
    region/region_manager.cpp
    region/region_stream_erosion.cpp
    region/region_border_stitcher.cpp
)

target_link_libraries(orogena_region
    PUBLIC
        orogena_core
        orogena_global
)

# Local detail library (Scale 3)
add_library(orogena_local
    local/local_detail_synthesizer.cpp
    local/local_geo_aware_noise.cpp
)

target_link_libraries(orogena_local
    PUBLIC
        orogena_region
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

# GUI library
add_library(orogena_gui
    gui/gui_main_window.cpp
    gui/gui_global_viewport.cpp
)

target_link_libraries(orogena_gui
    PUBLIC
        Qt6::Widgets
        orogena_rendering
)

# Database library
add_library(orogena_database
    database/database_manager.cpp
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
        orogena_gui
        orogena_database
        orogena_utils
)

# Set executable properties
set_target_properties(orogena PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
