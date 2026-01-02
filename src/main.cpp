/**************************************************************************************************/
/**
 * @file main.cpp
 * @brief Implementation of Main
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
/**************************************************************************************************/

#include <QApplication>
#include <QSurfaceFormat>
#include <qtdeprecationdefinitions.h>
#include <qtenvironmentvariables.h>

#include "database/database_manager.h"
#include "gui/gui_main_window.h"
#include "utils/utils_logger.h"
#include "utils/utils_types.h"

//=================================================================================================
// Static Member Initialization
//=================================================================================================

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

//=================================================================================================
// Public Functions
//=================================================================================================

int32_t main(int32_t argc, char_t* argv[])
{
    // Set default OpenGL format BEFORE creating QApplication
    // This is critical for both X11/GLX and Wayland/EGL
    QSurfaceFormat default_format;
    default_format.setVersion(4, 6); // OpenGL 4.5 Core
    default_format.setProfile(QSurfaceFormat::CoreProfile);
    default_format.setDepthBufferSize(24);
    default_format.setStencilBufferSize(8);
    default_format.setSamples(4); // 4x MSAA
    default_format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    default_format.setSwapInterval(1); // VSync
    default_format.setRenderableType(QSurfaceFormat::OpenGL);
    QSurfaceFormat::setDefaultFormat(default_format);

    // Initialize application
    QApplication app(argc, argv);

    QApplication::setApplicationName("Orogena");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("Orogena");

    // Initialize logger
    Orogena::Utils::Logger::Initialize();

    // Create and show main window
    Orogena::GUI::MainWindow main_window;
    main_window.show();

    int32_t result = QApplication::exec();

    // Clean up singletons before QApplication is destroyed
    // DatabaseManager uses QSqlDatabase which requires QCoreApplication to exist
    Orogena::Database::DatabaseManager::DestroyInstance();

    // Note: Logger::Shutdown() is NOT called here because main_window's destructor
    // runs after this point (during stack unwinding) and may still log.
    // The logger will clean up safely during static destruction.

    return result;
}

//=================================================================================================
// Private Functions
//=================================================================================================
