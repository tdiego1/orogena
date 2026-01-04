/**************************************************************************************************/
/**
 * @file gui_main_window.cpp
 * @brief Implementation of MainWindow
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

#include "gui/gui_main_window.h"

#include <QCloseEvent>
#include <QDockWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

#include "database/database_manager.h"
#include "render/render_viewport.h"
#include "utils/utils_logger.h"

namespace Orogena::GUI
{

//=================================================================================================
// Static Member Initialization
//=================================================================================================

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_ParametersDock(nullptr), m_PropertiesDock(nullptr)
{
    SetupProjectManager();
    SetupUI();
    UpdateWindowTitle();
    UpdateProjectUI();
    Log::Info("MainWindow created");
}

MainWindow::~MainWindow()
{
    Log::Info("MainWindow destroyed");
}

//=================================================================================================
// Protected Functions
//=================================================================================================

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (CheckUnsavedChanges())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

//=================================================================================================
// Private Functions
//=================================================================================================

void MainWindow::SetupUI()
{
    setWindowTitle("Orogena");
    resize(1440, 900);

    // Set window icon with multiple sizes for different contexts
    QIcon app_icon;
    app_icon.addFile(":/icons/icons/app_icon_16.png");
    app_icon.addFile(":/icons/icons/app_icon_32.png");
    app_icon.addFile(":/icons/icons/app_icon_48.png");
    app_icon.addFile(":/icons/icons/app_icon_64.png");
    app_icon.addFile(":/icons/icons/app_icon_128.png");
    app_icon.addFile(":/icons/icons/app_icon_256.png");
    app_icon.addFile(":/icons/icons/app_icon_512.png");
    setWindowIcon(app_icon);

    // Debug: Check if icon is null
    if (app_icon.isNull())
    {
        Log::Error("Failed to load application icon - check resources.qrc");
    }
    else
    {
        Log::Debug("Application icon loaded successfully with {} sizes",
                   app_icon.availableSizes().size());
    }

    SetupMenuBar();
    SetupToolBar();
    SetupDockPanels();
    SetupStatusBar();
    SetupViewport();
}

void MainWindow::SetupMenuBar()
{
    // File menu
    auto* file_menu = menuBar()->addMenu(tr("&File"));

    auto* action_new = file_menu->addAction(tr("&New Project..."));
    action_new->setShortcut(QKeySequence::New);
    connect(action_new, &QAction::triggered, this, &MainWindow::OnNewProject);

    auto* action_open = file_menu->addAction(tr("&Open Project..."));
    action_open->setShortcut(QKeySequence::Open);
    connect(action_open, &QAction::triggered, this, &MainWindow::OnOpenProject);

    // Recent Projects submenu
    m_RecentProjectsMenu = file_menu->addMenu(tr("Open &Recent"));
    UpdateRecentProjectsMenu();

    file_menu->addSeparator();

    m_ActionSave = file_menu->addAction(tr("&Save"));
    m_ActionSave->setShortcut(QKeySequence::Save);
    connect(m_ActionSave, &QAction::triggered, this, &MainWindow::OnSaveProject);

    m_ActionSaveAs = file_menu->addAction(tr("Save &As..."));
    m_ActionSaveAs->setShortcut(QKeySequence::SaveAs);
    connect(m_ActionSaveAs, &QAction::triggered, this, &MainWindow::OnSaveProjectAs);

    file_menu->addSeparator();
    auto* action_exit = file_menu->addAction(tr("E&xit"));
    action_exit->setShortcut(QKeySequence::Quit);
    connect(action_exit, &QAction::triggered, this, &QWidget::close);

    // Edit menu
    auto* edit_menu = menuBar()->addMenu(tr("&Edit"));
    edit_menu->addAction(tr("&Undo"));
    edit_menu->addAction(tr("&Redo"));
    edit_menu->addSeparator();
    edit_menu->addAction(tr("Cu&t"));
    edit_menu->addAction(tr("&Copy"));
    edit_menu->addAction(tr("&Paste"));
    edit_menu->addSeparator();
    edit_menu->addAction(tr("&Preferences..."));

    // View menu
    auto* view_menu = menuBar()->addMenu(tr("&View"));
    view_menu->addAction(tr("&Global View"));
    view_menu->addAction(tr("&Regional View"));
    view_menu->addAction(tr("&Local View"));
    view_menu->addSeparator();

    // Rendering options
    m_ActionWireframe = view_menu->addAction(tr("&Wireframe Mode"));
    m_ActionWireframe->setCheckable(true);
    m_ActionWireframe->setChecked(false);
    m_ActionWireframe->setShortcut(QKeySequence(tr("W")));
    connect(m_ActionWireframe, &QAction::triggered, this, &MainWindow::OnToggleWireframe);

    view_menu->addSeparator();

    // Add dock visibility toggles to View menu
    view_menu->addAction(tr("Show &Parameters Panel"), [this]()
                         { m_ParametersDock->setVisible(!m_ParametersDock->isVisible()); });
    view_menu->addAction(tr("Show &Properties Panel"), [this]()
                         { m_PropertiesDock->setVisible(!m_PropertiesDock->isVisible()); });

    // Simulation menu
    auto* sim_menu = menuBar()->addMenu(tr("&Simulation"));
    sim_menu->addAction(tr("&Start"));
    sim_menu->addAction(tr("&Pause"));
    sim_menu->addAction(tr("&Reset"));

    // Help menu
    auto* help_menu = menuBar()->addMenu(tr("&Help"));
    help_menu->addAction(tr("&Documentation"));
    help_menu->addAction(tr("&About Orogena"), this, &MainWindow::ShowAboutDialog);
}

void MainWindow::SetupToolBar()
{
    auto* toolbar = addToolBar(tr("Main Toolbar"));

    auto* action_new = toolbar->addAction(tr("New"));
    connect(action_new, &QAction::triggered, this, &MainWindow::OnNewProject);

    auto* action_open = toolbar->addAction(tr("Open"));
    connect(action_open, &QAction::triggered, this, &MainWindow::OnOpenProject);

    auto* action_save = toolbar->addAction(tr("Save"));
    connect(action_save, &QAction::triggered, this, &MainWindow::OnSaveProject);

    toolbar->addSeparator();
    toolbar->addAction(tr("Play"));
    toolbar->addAction(tr("Pause"));
    toolbar->addAction(tr("Stop"));
}

void MainWindow::SetupStatusBar()
{
    // Main status message (left side)
    statusBar()->showMessage(tr("Ready"));

    // Permenant widget for FPS (right side)
    m_StatusLabel = new QLabel(tr("FPS: 60"));
    m_StatusLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusBar()->addPermanentWidget(m_StatusLabel);
}

void MainWindow::SetupDockPanels()
{
    // Left sidebar - Parameters Panel
    m_ParametersDock = new QDockWidget(tr("Parameters"), this);
    m_ParametersDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    // Create placeholder content for parameters
    auto* params_widget = new QWidget();
    auto* params_layout = new QVBoxLayout(params_widget);
    params_layout->addWidget(new QLabel(tr("Simulation Parameters")));
    params_layout->addWidget(new QLabel(tr("(Coming in Phase 3)")));
    params_layout->addStretch();

    m_ParametersDock->setWidget(params_widget);
    addDockWidget(Qt::LeftDockWidgetArea, m_ParametersDock);

    // Right sidebar - Properties Panel
    m_PropertiesDock = new QDockWidget(tr("Properties"), this);
    m_PropertiesDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    // Create placeholder content for properties
    auto* props_widget = new QWidget();
    auto* props_layout = new QVBoxLayout(props_widget);
    props_layout->addWidget(new QLabel(tr("Properties & Info")));
    props_layout->addWidget(new QLabel(tr("(Coming in Phase 2)")));
    props_layout->addStretch();

    m_PropertiesDock->setWidget(props_widget);
    addDockWidget(Qt::RightDockWidgetArea, m_PropertiesDock);

    Log::Debug("Dockable panels initialized: Parameters (left), Properties (right)");
}

void MainWindow::SetupViewport()
{
    // Create and set viewport as central widget
    m_Viewport = new Render::Viewport(this);
    setCentralWidget(m_Viewport);

    // Connect FPS update to status bar
    connect(m_Viewport, &Render::Viewport::FPSUpdated, this,
            [this](int32_t fps) { m_StatusLabel->setText(QString("FPS: %1").arg(fps)); });

    // Connect OpenGL initialization
    connect(m_Viewport, &Render::Viewport::OpenGLInitialized, this,
            [](const std::string& vendor, const std::string& renderer, const std::string& version)
            { Log::Info("MainWindow: GPU initialized - {} / {}", vendor, renderer); });

    // Connect OpenGL errors
    connect(m_Viewport, &Render::Viewport::OpenGLError, this,
            [this](const std::string& error)
            {
                QMessageBox::critical(
                    this, "OpenGL Error",
                    QString("OpenGL error occurred:\n%1").arg(QString::fromStdString(error)));
            });
}

void MainWindow::SetupProjectManager()
{
    // Create settings and project manager with database support
    m_Settings = std::make_unique<QtSettings>();
    m_ProjectManager =
        std::make_unique<Core::ProjectManager>(*m_Settings, &Database::DatabaseManager::Instance());

    // Setup callbacks
    Core::ProjectCallbacks callbacks;

    callbacks.onProjectOpened = [this](const std::string& path)
    {
        UpdateWindowTitle();
        UpdateProjectUI();
        UpdateRecentProjectsMenu();
        statusBar()->showMessage(tr("Project opened: %1").arg(QString::fromStdString(path)), 3000);
    };

    callbacks.onProjectClosed = [this]()
    {
        UpdateWindowTitle();
        UpdateProjectUI();
        statusBar()->showMessage(tr("Project closed"), 3000);
    };

    callbacks.onProjectModified = [this]() { UpdateWindowTitle(); };

    callbacks.onProjectSaved = [this](const std::string& path)
    {
        UpdateWindowTitle();
        statusBar()->showMessage(tr("Project saved: %1").arg(QString::fromStdString(path)), 3000);
    };

    callbacks.onError = [this](const std::string& error)
    { QMessageBox::warning(this, tr("Project Error"), QString::fromStdString(error)); };

    m_ProjectManager->SetCallbacks(callbacks);
}

void MainWindow::UpdateRecentProjectsMenu()
{
    m_RecentProjectsMenu->clear();

    auto recent = m_ProjectManager->GetRecentProjects();

    if (recent.empty())
    {
        auto* no_recent = m_RecentProjectsMenu->addAction(tr("(No recent projects)"));
        no_recent->setEnabled(false);
    }
    else
    {
        for (const auto& path : recent)
        {
            QString path_str = QString::fromStdString(path.string());
            QString display_name = QString::fromStdString(path.filename().string());

            auto* action = m_RecentProjectsMenu->addAction(display_name);
            action->setToolTip(path_str);
            connect(action, &QAction::triggered, this,
                    [this, path_str]() { OnOpenRecentProject(path_str); });
        }

        m_RecentProjectsMenu->addSeparator();
        auto* clear_action = m_RecentProjectsMenu->addAction(tr("Clear Recent Projects"));
        connect(clear_action, &QAction::triggered, this,
                [this]()
                {
                    m_ProjectManager->ClearRecentProjects();
                    UpdateRecentProjectsMenu();
                });
    }
}

void MainWindow::UpdateWindowTitle()
{
    QString title = "Orogena";

    if (m_ProjectManager->IsProjectOpen())
    {
        auto info = m_ProjectManager->GetProjectInfo();
        if (info)
        {
            title = QString::fromStdString(info->name) + " - Orogena";

            if (m_ProjectManager->HasUnsavedChanges())
            {
                title = "*" + title;
            }
        }
    }

    setWindowTitle(title);
}

void MainWindow::UpdateProjectUI()
{
    bool project_open = m_ProjectManager->IsProjectOpen();

    m_ActionSave->setEnabled(project_open);
    m_ActionSaveAs->setEnabled(project_open);
}

bool MainWindow::CheckUnsavedChanges()
{
    if (!m_ProjectManager->HasUnsavedChanges())
    {
        return true;
    }

    auto    info = m_ProjectManager->GetProjectInfo();
    QString project_name = info ? QString::fromStdString(info->name) : tr("Untitled");

    QMessageBox::StandardButton result = QMessageBox::question(
        this, tr("Unsaved Changes"),
        tr("The project \"%1\" has unsaved changes.\n\nDo you want to save before closing?")
            .arg(project_name),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);

    switch (result)
    {
        case QMessageBox::Save:
            return m_ProjectManager->SaveProject();
        case QMessageBox::Discard:
            return m_ProjectManager->CloseProject(true);
        case QMessageBox::Cancel:
        default:
            return false;
    }
}

//=================================================================================================
// Private Slots
//=================================================================================================

void MainWindow::ShowAboutDialog()
{
    QString about_text =
        tr("<h2>Orogena</h2>"
           "<p><b>Complete Worldbuilding Suite</b></p>"
           "<p>Version 1.0.0 (Phase 0)</p>"
           "<p>Copyright (C) 2025 Diego Torres. All rights reserved.</p>"
           "<p>Licensed under the GNU General Public License v3.0</p>"
           "<hr>"
           "<p>Orogena is a cross-platform desktop application for complete "
           "worldbuilding following Artifexian's methodology. It targets fantasy "
           "authors, game developers, and world-builders who need geologically and "
           "climatologically plausible worlds from stellar parameters down to local detail.</p>"
           "<p><b>Technology Stack:</b><br>"
           "- C++20<br>"
           "- Qt 6.8+<br>"
           "- OpenGL 4.6+<br>"
           "- SQLite</p>"
           "<p>For more information, visit the project documentation.</p>");

    QMessageBox::about(this, tr("About Orogena"), about_text);
    Log::Debug("About dialog displayed");
}

void MainWindow::OnNewProject()
{
    // Check for unsaved changes first
    if (!CheckUnsavedChanges())
    {
        return;
    }

    // Close existing project
    if (m_ProjectManager->IsProjectOpen())
    {
        m_ProjectManager->CloseProject(true);
    }

    // Get project name
    bool    ok;
    QString name = QInputDialog::getText(this, tr("New Project"), tr("Project name:"),
                                         QLineEdit::Normal, tr("My World"), &ok);

    if (!ok || name.isEmpty())
    {
        return;
    }

    // Use default projects directory (~/Documents/Orogena)
    std::string default_dir = m_Settings->GetDefaultProjectsDirectory();

    // Create project in the default directory (will create ProjectName subdirectory)
    if (!m_ProjectManager->CreateProject(default_dir, name.toStdString()))
    {
        // Error callback will have shown the message
        return;
    }

    Log::Info("New project created: {}", name.toStdString());
}

void MainWindow::OnOpenProject()
{
    // Check for unsaved changes first
    if (!CheckUnsavedChanges())
    {
        return;
    }

    // Close existing project
    if (m_ProjectManager->IsProjectOpen())
    {
        m_ProjectManager->CloseProject(true);
    }

    // Get file path (start in default projects directory)
    std::string default_dir = m_Settings->GetDefaultProjectsDirectory();
    QString     file_path =
        QFileDialog::getOpenFileName(this, tr("Open Project"), QString::fromStdString(default_dir),
                                     tr("Orogena Projects (*.oro);;All Files (*)"));

    if (file_path.isEmpty())
    {
        return;
    }

    // Open project
    if (!m_ProjectManager->OpenProject(file_path.toStdString()))
    {
        // Error callback will have shown the message
        return;
    }

    Log::Info("Project opened: {}", file_path.toStdString());
}

void MainWindow::OnOpenRecentProject(const QString& path)
{
    // Check for unsaved changes first
    if (!CheckUnsavedChanges())
    {
        return;
    }

    // Close existing project
    if (m_ProjectManager->IsProjectOpen())
    {
        m_ProjectManager->CloseProject(true);
    }

    // Check if file exists
    if (!std::filesystem::exists(path.toStdString()))
    {
        QMessageBox::warning(this, tr("File Not Found"),
                             tr("The project file could not be found:\n%1").arg(path));

        // Optionally remove from recent list
        UpdateRecentProjectsMenu();
        return;
    }

    // Open project
    if (!m_ProjectManager->OpenProject(path.toStdString()))
    {
        // Error callback will have shown the message
        return;
    }

    Log::Info("Recent project opened: {}", path.toStdString());
}

void MainWindow::OnSaveProject()
{
    if (!m_ProjectManager->IsProjectOpen())
    {
        return;
    }

    m_ProjectManager->SaveProject();
}

void MainWindow::OnSaveProjectAs()
{
    if (!m_ProjectManager->IsProjectOpen())
    {
        return;
    }

    auto info = m_ProjectManager->GetProjectInfo();

    // Build default path in the projects directory
    std::string default_dir = m_Settings->GetDefaultProjectsDirectory();
    QString     default_name = info ? QString::fromStdString(info->name) + ".oro" : "project.oro";
    QString     default_path = QString::fromStdString(default_dir) + "/" + default_name;

    QString file_path = QFileDialog::getSaveFileName(this, tr("Save Project As"), default_path,
                                                     tr("Orogena Projects (*.oro);;All Files (*)"));

    if (file_path.isEmpty())
    {
        return;
    }

    // Ensure .oro extension
    if (!file_path.endsWith(".oro", Qt::CaseInsensitive))
    {
        file_path += ".oro";
    }

    m_ProjectManager->SaveProjectAs(file_path.toStdString());
}

void MainWindow::OnToggleWireframe(bool checked)
{
    if (m_Viewport)
    {
        m_Viewport->SetWireframeMode(checked);
    }
}

} // namespace Orogena::GUI
