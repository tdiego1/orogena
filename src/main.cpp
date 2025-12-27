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
