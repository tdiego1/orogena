/**************************************************************************************************/
/**
 * @file main.cpp
 * @brief Implementation of Main
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres. All rights reserved.
 */
/**************************************************************************************************/

#include <QApplication>
#include <QSurfaceFormat>
#include <qtdeprecationdefinitions.h>
#include <qtenvironmentvariables.h>

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

    return QApplication::exec();
}

//=================================================================================================
// Private Functions
//=================================================================================================
