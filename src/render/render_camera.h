/**************************************************************************************************/
/**
 * @file render_camera.h
 * @brief Camera management for 3D viewport navigation
 *
 * @details
 * Implements an arcball/orbital camera for navigating the 3D world view.
 * Supports pan, zoom, and rotation controls with smooth interpolation.
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

#include <glm/glm.hpp>

#include "utils/utils_types.h"

namespace Orogena::Render
{
/**************************************************************************************************/
/**
 * @brief Orbital camera for 3D viewport navigation
 *
 * @details
 * Provides arcball-style camera controls centered on a target point.
 * - Pan: Moves the target point
 * - Zoom: Adjusts distance from target
 * - Rotate: Orbits around target using spherical coordinates
 */
class Camera
{
  public:
    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    /**
     * @brief Construct a new Camera with default settings
     */
    Camera();

    ~Camera() = default;

    // Delete copy operations
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;

    // Default move operations
    Camera(Camera&&) = default;
    Camera& operator=(Camera&&) = default;

    //=============================================================================================
    // Public Functions
    //=============================================================================================

    /**
     * @brief Get the view matrix for rendering
     *
     * @return glm::mat4 View matrix transforming world space to camera space.
     */
    [[nodiscard]] glm::mat4 GetViewMatrix() const;

    /**
     * @brief Get the projection matrix for rendering
     *
     * @return glm::mat4 Projection matrix transforming camera space to clip space.
     */
    [[nodiscard]] glm::mat4 GetProjectionMatrix() const
    {
        return m_ProjectionMatrix;
    }

    /**
     * @brief Update projection matrix for new viewport dimensions
     *
     * @param widthPx Viewport width in pixels.
     * @param heightPx Viewport height in pixels.
     */
    void SetViewportSize(int32_t widthPx, int32_t heightPx);

    /**
     * @brief Pan the camera (move target point)
     *
     * @param deltaX Horizontal movement in normalized coordinates.
     * @param deltaY Vertical movement in normalized coordinates.
     */
    void Pan(float32_t deltaX, float32_t deltaY);

    /**
     * @brief Zoom the camera (adjust distance from target)
     *
     * @param delta Zoom delta (positive = zoom in, negative = zoom out).
     */
    void Zoom(float32_t delta);

    /**
     * @brief Rotate the camera around the target
     *
     * @param deltaAzimuth Horizontal rotation in radians.
     * @param deltaPolar Vertical rotation in radians.
     */
    void Rotate(float32_t deltaAzimuth, float32_t deltaPolar);

    /**
     * @brief Reset camera to default position
     */
    void Reset();

    /**
     * @brief Get current camera distance from target
     *
     * @return float32_t Distance in world units.
     */
    [[nodiscard]] float32_t GetDistance() const
    {
        return m_Distance;
    }

    /**
     * @brief Get camera target position
     *
     * @return glm::vec3 Target point in world space.
     */
    [[nodiscard]] glm::vec3 GetTarget() const
    {
        return m_Target;
    }

  private:
    //=============================================================================================
    // Private Functions
    //=============================================================================================

    /**
     * @brief Update projection matrix based on current settings
     */
    void UpdateProjectionMatrix();

    //=============================================================================================
    // Private Members
    //=============================================================================================

    // Camera parameters
    glm::vec3 m_Target{0.0F, 0.0F, 0.0F}; ///< Look-at target position
    float32_t m_Distance{10.0F};          ///< Distance from target
    float32_t m_Azimuth{0.0F};            ///< Horizontal rotation (radians)
    float32_t m_Polar{0.785F};            ///< Vertical rotation (radians, 45 deg default)

    // Projection parameters
    glm::mat4 m_ProjectionMatrix{1.0F};    ///< Cached projection matrix
    float32_t m_AspectRatio{16.0F / 9.0F}; ///< Viewport aspect ratio
    float32_t m_FieldOfView{45.0F};        ///< Vertical FOV in degrees
    float32_t m_NearPlane{0.1F};           ///< Near clipping plane
    float32_t m_FarPlane{1000.0F};         ///< Far clipping plane

    // Control sensitivity
    static constexpr float32_t c_PanSpeed = 5.0F;
    static constexpr float32_t c_ZoomSpeed = 1.2F;
    static constexpr float32_t c_RotateSpeed = 0.005F;
    static constexpr float32_t c_MinDistance = 1.0F;
    static constexpr float32_t c_MaxDistance = 100.0F;
    static constexpr float32_t c_MinPolar = 0.1F;  // Prevent gimbal lock
    static constexpr float32_t c_MaxPolar = 3.04F; // ~174 degrees
};

} // namespace Orogena::Render
