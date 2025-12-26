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

#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>

#include <spdlog/spdlog.h>

namespace Orogena
{
namespace GUI
{

//=================================================================================================
// Static Member Initialization
//=================================================================================================

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    SetupUI();
    spdlog::info("MainWindow created");
}

MainWindow::~MainWindow()
{
    spdlog::info("MainWindow destroyed");
}

//=================================================================================================
// Public Functions
//=================================================================================================

void MainWindow::SetupUI(void)
{
    setWindowTitle("Orogena - Multi-Scale Tectonic Simulator");
    resize(1280, 720);

    SetupMenuBar();
    SetupToolBar();
    SetupStatusBar();

    // TODO: Add central widget (viewport)
    auto* placeholder = new QLabel("Orogena v1.0\n\nProject structure initialized!\n\nSee docs/ for development plan.", this);
    placeholder->setAlignment(Qt::AlignCenter);
    setCentralWidget(placeholder);
}

void MainWindow::SetupMenuBar(void)
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

    // Simulation menu
    auto* sim_menu = menuBar()->addMenu(tr("&Simulation"));
    sim_menu->addAction(tr("&Start"));
    sim_menu->addAction(tr("&Pause"));
    sim_menu->addAction(tr("&Reset"));

    // View menu
    auto* view_menu = menuBar()->addMenu(tr("&View"));
    view_menu->addAction(tr("&Global View"));
    view_menu->addAction(tr("&Regional View"));
    view_menu->addAction(tr("&Local View"));

    // Help menu
    auto* help_menu = menuBar()->addMenu(tr("&Help"));
    help_menu->addAction(tr("&Documentation"));
    help_menu->addAction(tr("&About Orogena"));
}

void MainWindow::SetupToolBar(void)
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

void MainWindow::SetupStatusBar(void)
{
    statusBar()->showMessage(tr("Ready"));
}

//=================================================================================================
// Private Functions
//=================================================================================================

} // namespace GUI
} // namespace Orogena