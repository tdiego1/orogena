/**************************************************************************************************/
/**
 * @file gui_main_window.cpp
 * @brief Implementation of MainWindow
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres. All rights reserved.
 */
/**************************************************************************************************/

#include "gui/gui_main_window.h"

#include <QDockWidget>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include <qboxlayout.h>
#include <qdockwidget.h>
#include <qlabel.h>
#include <qnamespace.h>

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
    SetupUI();
    Log::Info("MainWindow created");
}

MainWindow::~MainWindow()
{
    Log::Info("MainWindow destroyed");
}

//=================================================================================================
// Public Functions
//=================================================================================================

void MainWindow::SetupUI()
{
    setWindowTitle("Orogena - Multi-Scale Tectonic Simulator");
    resize(1280, 720);

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

    // TODO: Replace with QOpenGLWidget viewport
    auto* placeholder = new QLabel(
        "Orogena v1.0\n\nProject structure initialized!\n\nSee docs/ for development plan.", this);
    placeholder->setAlignment(Qt::AlignCenter);
    setCentralWidget(placeholder);
}

void MainWindow::SetupMenuBar()
{
    // File menu
    auto* file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction(tr("&New Project..."));
    file_menu->addAction(tr("&Open Project..."));
    file_menu->addSeparator();
    file_menu->addAction(tr("&Save"));
    file_menu->addAction(tr("Save &As..."));
    file_menu->addSeparator();
    file_menu->addAction(tr("E&xit"), this, &QWidget::close);

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
    toolbar->addAction(tr("New"));
    toolbar->addAction(tr("Open"));
    toolbar->addAction(tr("Save"));
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
    auto* fps_label = new QLabel(tr("FPS: 60"));
    fps_label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusBar()->addPermanentWidget(fps_label);
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

void MainWindow::ShowAboutDialog()
{
    QString about_text =
        tr("<h2>Orogena</h2>"
           "<p><b>Multi-Scale Terrain Generation through Tectonic Simulation</b></p>"
           "<p>Version 1.0.0 (Phase 1 - Foundation)</p>"
           "<p>Copyright © 2025 Diego Torres. All rights reserved.</p>"
           "<p>Licensed under the GNU General Public License v3.0</p>"
           "<hr>"
           "<p>Orogena is a cross-platform desktop application for generating realistic terrain "
           "through geologically accurate plate tectonic simulation.</p>"
           "<p><b>Technology Stack:</b><br>"
           "• C++20<br>"
           "• Qt 6.8+<br>"
           "• OpenGL 4.5+<br>"
           "• SQLite</p>"
           "<p>For more information, visit the project documentation.</p>");

    QMessageBox::about(this, tr("About Orogena"), about_text);
    Log::Debug("About dialog displayed");
}

//=================================================================================================
// Private Functions
//=================================================================================================

} // namespace Orogena::GUI