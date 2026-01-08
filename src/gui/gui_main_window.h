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

#pragma once

#include <memory>

#include <QLabel>
#include <QMainWindow>
#include <QMenu>

#include "core/core_project.h"
#include "database/database_manager.h"
#include "gui/gui_qt_settings.h"
#include "render/render_viewport.h"

// Forward declarations
class QDockWidget;
class QAction;

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

  protected:
    //=============================================================================================
    // Protected Functions (Qt overrides)
    //=============================================================================================

    /**
     * @brief Handle window close event
     * @param event Close event to handle
     */
    void closeEvent(QCloseEvent* event) override;

  private slots:
    //=============================================================================================
    // Private Slots
    //=============================================================================================

    /**
     * @brief Show the About dialog.
     */
    void ShowAboutDialog();

    /**
     * @brief Create a new project.
     */
    void OnNewProject();

    /**
     * @brief Open an existing project.
     */
    void OnOpenProject();

    /**
     * @brief Open a recent project.
     * @param path Path to the project file
     */
    void OnOpenRecentProject(const QString& path);

    /**
     * @brief Save the current project.
     */
    void OnSaveProject();

    /**
     * @brief Save the current project to a new location.
     */
    void OnSaveProjectAs();

    /**
     * @brief Toggle wireframe rendering mode.
     * @param checked True for wireframe, false for solid
     */
    void OnToggleWireframe(bool checked);

    /**
     * @brief Display Galaxy view.
     */
    void OnGalaxyClicked();

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

    /**
     * @brief Setup the project manager and callbacks.
     */
    void SetupProjectManager();

    /**
     * @brief Update the recent projects submenu.
     */
    void UpdateRecentProjectsMenu();

    /**
     * @brief Update window title based on project state.
     */
    void UpdateWindowTitle();

    /**
     * @brief Update UI state based on whether a project is open.
     */
    void UpdateProjectUI();

    /**
     * @brief Check for unsaved changes and prompt user.
     * @return true if safe to proceed (saved or user chose to discard)
     */
    bool CheckUnsavedChanges();

    //=============================================================================================
    // Private Member Variables
    //=============================================================================================

    QDockWidget* m_ParametersDock; ///< Left sidebar for simulation parameters.
    QDockWidget* m_PropertiesDock; ///< Right sidebar for properties/info.

    QLabel* m_StatusLabel; ///< Status bar label for FPS display.

    Render::Viewport* m_Viewport{nullptr}; ///< Central OpenGL rendering viewport.

    // Project management
    std::unique_ptr<QtSettings>           m_Settings;       ///< Application settings.
    std::unique_ptr<Core::ProjectManager> m_ProjectManager; ///< Project lifecycle manager.

    // File menu actions (need references for enable/disable)
    QAction* m_ActionSave{nullptr};         ///< Save action.
    QAction* m_ActionSaveAs{nullptr};       ///< Save As action.
    QMenu*   m_RecentProjectsMenu{nullptr}; ///< Recent projects submenu.

    // View menu actions
    QAction* m_ActionWireframe{nullptr}; ///< Wireframe toggle action.
};

} // namespace Orogena::GUI
