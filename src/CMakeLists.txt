#[[************************************************************************************************/
/**
 * @brief CMake configuration for the Orogena project.
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * For commercial licensing options, please contact: tdiego001@gmail.com
 */
/************************************************************************************************]]#

#===================================================================================================
# Core library (shared by all components)
#===================================================================================================

add_library(orogena_core
    core/core_application.cpp
    core/core_project.cpp
    core/core_settings.cpp
)

target_link_libraries(orogena_core
    PUBLIC
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)

#===================================================================================================
# Stellar system library
#===================================================================================================

add_library(orogena_stellar
    stellar/stellar_star.cpp
    stellar/stellar_system.cpp
    stellar/stellar_habitable_zone.cpp
)

target_link_libraries(orogena_stellar
    PUBLIC
        orogena_core
)

#===================================================================================================
# Global simulation library (Scale 1)
#===================================================================================================

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

#===================================================================================================
# Region generation library (Scale 2)
#===================================================================================================

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

#===================================================================================================
# Local detail library (Scale 3)
#===================================================================================================

add_library(orogena_local
    local/local_detail_synthesizer.cpp
    local/local_geo_aware_noise.cpp
)

target_link_libraries(orogena_local
    PUBLIC
        orogena_region
)

#===================================================================================================
# Render library
#===================================================================================================

add_library(orogena_render
    render/render_multiscale.cpp
    render/render_heightmap.cpp
    render/render_camera.cpp
    render/render_grid.cpp
    render/render_mesh.cpp
    render/render_sphere.cpp
    render/render_viewport.cpp
)

target_link_libraries(orogena_render
    PUBLIC
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        OpenGL::GL
        orogena_local
)

#===================================================================================================
# GUI library
#===================================================================================================

# GUI library
add_library(orogena_gui
    gui/gui_main_window.cpp
    gui/gui_main_window.h
    gui/gui_global_viewport.cpp
    gui/gui_global_viewport.h
    gui/gui_qt_settings.cpp
    gui/gui_qt_settings.h
)

target_link_libraries(orogena_gui
    PUBLIC
        Qt6::Widgets
        orogena_render
)

#===================================================================================================
# Database library
#===================================================================================================

# Database library
add_library(orogena_database
    database/database_manager.cpp
    database/database_transaction.cpp
)

target_link_libraries(orogena_database
    PUBLIC
        Qt6::Sql
        orogena_core
        orogena_utils
)

#===================================================================================================
# Utilities library
#===================================================================================================

# Utilities library
add_library(orogena_utils
    utils/utils_logger.cpp
    utils/utils_types.h
)

target_link_libraries(orogena_utils
    PUBLIC
        spdlog::spdlog
)

#===================================================================================================
# Main executable
#===================================================================================================

add_executable(orogena
    main.cpp
)

# Qt resources - added to executable to ensure they're linked
target_sources(orogena PRIVATE
    ${CMAKE_SOURCE_DIR}/resources/resources.qrc
)

# Enable Qt's automatic resource compiler
set_target_properties(orogena PROPERTIES
    AUTORCC ON
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

target_link_libraries(orogena
    PRIVATE
        orogena_gui
        orogena_database
        orogena_utils
)
