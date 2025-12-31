/**************************************************************************************************/
/**
 * @file render_viewport.h
 * @brief OpenGL rendering context for rendering the simulator
 *
 * @details
 * QOpenGLWidget-based viewport that provides OpenGL 4.5+ rendering context with fallback to 3.3.
 * Displays clear color background and tracks FPS for performance monitoring.
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

#include <memory>
#include <string>

#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLWidget>

#include "render_camera.h"
#include "render_grid.h"
#include "utils/utils_types.h"

#include <chrono>

namespace Orogena::Render
{
/**************************************************************************************************/
/**
 * @brief OpenGL rendering context for rendering the tectonic simulator
 *
 * @details
 * Manages OpenGL initialization, rendering loop, and viewport management.
 * Provides FPS tracking and emits signals for status updates.
 */
class Viewport : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

  public:
    //=============================================================================================
    // Public Types
    //=============================================================================================

    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    /**
     * @brief Contruct a new Viewport
     *
     * @param parent Parent widget (typically MainWindow).
     */
    explicit Viewport(QWidget* parent = nullptr);

    ~Viewport();

    // Delete copy operations
    Viewport(const Viewport&) = delete;
    Viewport& operator=(const Viewport&) = delete;

    // Default move operations
    Viewport(Viewport&&) = delete;
    Viewport& operator=(Viewport&&) = delete;

    //=============================================================================================
    // Public Functions
    //=============================================================================================

    /**
     * @brief Get current frames per second
     *
     * @return int32_t FPS value (updated every second).
     */
    [[nodiscard]] int32_t GetFPS() const
    {
        return m_CurrentFPS;
    };

    /**
     * @brief Set background clear color
     *
     * @param color RGB color structure.
     */
    void SetClearColor(Utils::ColorRGBF color);

  signals:
    /**
     * @brief Emitted when FPS counter updates (once per second)
     *
     * @param fps Current frames per second.
     */
    void FPSUpdated(int32_t fps);

    /**
     * @brief Emitted when OpenGL context is successfully initialized
     *
     * @param vendor GPU vendor string.
     * @param renderer GPU renderer string.
     * @param version OpenGL version string.
     */
    void OpenGLInitialized(const std::string& vendor,
                           const std::string& renderer,
                           const std::string& version);

    /**
     * @brief Emitted when OpenGL error occurs
     *
     * @param error Error message.
     */
    void OpenGLError(const std::string& error);

  protected:
    //=============================================================================================
    // Protected Functions
    //=============================================================================================

    /**
     * @brief Initialize OpenGL context and resources
     */
    void initializeGL() override;

    /**
     * @brief Render a new frame
     */
    void paintGL() override;

    /**
     * @brief Handle viewport resize
     *
     * @param widthPx New width in pixels.
     * @param heightPx New height in pixels.
     */
    void resizeGL(int32_t widthPx, int32_t heightPx) override;

    /**
     * @brief Handle mouse press events
     *
     * @param event Mouse event data.
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief Handle mouse move events
     *
     * @param event Mouse event data.
     */
    void mouseMoveEvent(QMouseEvent* event) override;

    /**
     * @brief Handle mouse release events
     *
     * @param event Mouse event data.
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

    /**
     * @brief Handle mouse wheel events
     *
     * @param event Wheel event data.
     */
    void wheelEvent(QWheelEvent* event) override;

  private:
    //=============================================================================================
    // Private Types
    //=============================================================================================

    //=============================================================================================
    // Private Functions
    //=============================================================================================

    /**
     * @brief Check for OpenGL errors and log them
     *
     * @param context Description of the current operation.
     * @return true An error occured.
     * @return false No error occure.
     */
    bool CheckGLError(const std::string& context);

    /**
     * @brief Update FPS counter
     */
    void UpdateFPS();

    //=============================================================================================
    // Private Members
    //=============================================================================================

    // Clear color (Sky blue by default)
    Utils::ColorRGBF m_ClearColor{.r = 0.53F, .g = 0.81F, .b = 0.92F};

    // FPS tracking
    std::chrono::steady_clock::time_point m_FrameStartTime; ///< Start time for frame timing.
    int32_t                               m_FrameCount{0};  ///< Frames rendered in current second.
    int32_t                               m_CurrentFPS{0};  ///< Current FPS value.
    int64_t m_LastFPSUpdateMs{0};                           ///< Timestamp of last FPS update (ms).

    // Camera and rendering
    std::unique_ptr<Camera> m_Camera; ///< Camera for viewport navigation.
    std::unique_ptr<Grid>   m_Grid;   ///< Reference grid renderer.

    // Mouse interaction state
    bool   m_LeftMousePressed{false};  ///< Left mouse button state.
    bool   m_RightMousePressed{false}; ///< Right mouse button state.
    QPoint m_LastMousePos;             ///< Last mouse position for delta calculation.
};

} // namespace Orogena::Render
