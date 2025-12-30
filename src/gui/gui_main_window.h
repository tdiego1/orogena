/**************************************************************************************************/
/**
 * @file gui_main_window.h
 * @brief Main application window.
 *
 * @details
 * Primary container for Orogena application.
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */
/**************************************************************************************************/

#pragma once

#include <QLabel>
#include <QMainWindow>

#include "render/render_viewport.h"

// Forward declarations
class QDockWidget;

namespace Orogena::GUI
{

/**************************************************************************************************/
/**
 * @brief Main application window
 *
 * @details
 * Primary UI container for Orogena application.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    //=============================================================================================
    // Public Types
    //=============================================================================================

    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

  private slots:
    //=============================================================================================
    // Private Slots
    //=============================================================================================

    /**
     * @brief Show the About dialog.
     */
    void ShowAboutDialog();

  private:
    //=============================================================================================
    // Private Types
    //=============================================================================================

    //=============================================================================================
    // Private Functions
    //=============================================================================================

    /**
     * @brief Setup the UI.
     */
    void SetupUI();

    /**
     * @brief Setup the menu bar.
     */
    void SetupMenuBar();

    /**
     * @brief Setup the tool bar.
     */
    void SetupToolBar();

    /**
     * @brief Setup the status bar.
     */
    void SetupStatusBar();

    /**
     * @brief Setup the dockable panels.
     */
    void SetupDockPanels();

    /**
     * @brief Setup the central viewport.
     */
    void SetupViewport();

    //=============================================================================================
    // Private Member Variables
    //=============================================================================================

    QDockWidget* m_ParametersDock; ///< Left sidebar for simulation parameters.
    QDockWidget* m_PropertiesDock; ///< Right sidebar for properties/info.

    QLabel* m_StatusLabel; ///< Status bar label for FPS display.

    Render::Viewport* m_Viewport{nullptr}; ///< Central OpenGL rendering viewport.
};

} // namespace Orogena::GUI
