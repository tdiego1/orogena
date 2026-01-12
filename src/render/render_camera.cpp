/**************************************************************************************************/
/**
 * @file render_camera.cpp
 * @brief Implementation of Camera
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

#include "render_camera.h"

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include "utils/utils_logger.h"

namespace Orogena::Render
{

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

Camera::Camera()
{
    UpdateProjectionMatrix();
    Log::Debug("Camera: Created with default settings (distance={}, FOV={}°)", m_Distance,
               m_FieldOfView);
}

//=================================================================================================
// Public Functions
//=================================================================================================

glm::mat4 Camera::GetViewMatrix() const
{
    // Convert spherical coordinates to Cartesian position
    // For galaxy view (XY plane): polar=0 means looking down Z axis (top-down view)
    // For 3D view: standard spherical coordinates
    float32_t x = m_Distance * std::sin(m_Polar) * std::cos(m_Azimuth);
    float32_t y = m_Distance * std::sin(m_Polar) * std::sin(m_Azimuth);
    float32_t z = m_Distance * std::cos(m_Polar);

    glm::vec3 camera_position = m_Target + glm::vec3(x, y, z);

    // Choose up vector to avoid gimbal lock
    // When looking straight down (polar ≈ 0), use -Y as up (galaxy XY plane, camera on +Z)
    // When looking straight up (polar ≈ π), use +Y as up (camera on -Z)
    glm::vec3 up_vector;
    if (m_Polar < 0.2F)
    {
        // Top-down view: camera on +Z axis looking down at XY plane, use -Y as up
        up_vector = glm::vec3(0.0F, -1.0F, 0.0F);
    }
    else if (m_Polar > 3.0F)
    {
        // Bottom-up view: camera on -Z axis looking up at XY plane, use +Y as up
        up_vector = glm::vec3(0.0F, 1.0F, 0.0F);
    }
    else
    {
        // Normal orbital view: use +Z as up (standard for XY plane viewing)
        up_vector = glm::vec3(0.0F, 0.0F, 1.0F);
    }

    return glm::lookAt(camera_position, m_Target, up_vector);
}

void Camera::SetViewportSize(int32_t widthPx, int32_t heightPx)
{
    if (heightPx > 0)
    {
        m_AspectRatio = static_cast<float32_t>(widthPx) / static_cast<float32_t>(heightPx);
        UpdateProjectionMatrix();
    }
}

void Camera::Pan(float32_t deltaX, float32_t deltaY)
{
    // Pan in the camera's local coordinate system
    glm::mat4 view_matrix = GetViewMatrix();

    // Extract right and up vectors from view matrix
    glm::vec3 right = glm::vec3(view_matrix[0][0], view_matrix[1][0], view_matrix[2][0]);
    glm::vec3 up = glm::vec3(view_matrix[0][1], view_matrix[1][1], view_matrix[2][1]);

    // Apply pan movement scaled by distance
    float32_t pan_scale = m_Distance * c_PanSpeed * 0.01F;
    m_Target += right * deltaX * pan_scale;
    m_Target += up * deltaY * pan_scale;
}

void Camera::Zoom(float32_t delta)
{
    if (m_UseOrthographic)
    {
        // For orthographic: scale the ortho size (FOV equivalent)
        float32_t zoom_factor = std::pow(c_ZoomSpeed, delta);
        m_OrthoSize *= zoom_factor;
        m_OrthoSize = std::clamp(m_OrthoSize, c_MinOrthoSize, c_MaxOrthoSize);
        UpdateProjectionMatrix();
    }
    else
    {
        // For perspective: change distance
        float32_t zoom_factor = std::pow(c_ZoomSpeed, delta);
        m_Distance *= zoom_factor;
        m_Distance = std::clamp(m_Distance, c_MinDistance, c_MaxDistance);
    }
}

void Camera::Rotate(float32_t deltaAzimuth, float32_t deltaPolar)
{
    // Update spherical coordinates
    m_Azimuth += deltaAzimuth * c_RotateSpeed;
    m_Polar += deltaPolar * c_RotateSpeed;

    // Clamp polar angle to prevent gimbal lock and flipping
    m_Polar = std::clamp(m_Polar, c_MinPolar, c_MaxPolar);

    // Normalize azimuth to [0, 2π]
    while (m_Azimuth < 0.0F)
    {
        m_Azimuth += 2.0F * 3.14159265F;
    }
    while (m_Azimuth > 2.0F * 3.14159265F)
    {
        m_Azimuth -= 2.0F * 3.14159265F;
    }
}

void Camera::Reset()
{
    m_Target = glm::vec3(0.0F, 0.0F, 0.0F);

    if (m_UseOrthographic)
    {
        // For orthographic galaxy view: top-down view
        m_Distance = 2.0F;
        m_Azimuth = 0.0F;
        m_Polar = 0.0F; // Look straight down (top view)
        m_OrthoSize = 15000.0F; // Start zoomed to show ~30000 parsec galaxy
    }
    else
    {
        // For perspective 3D view: orbital camera
        m_Distance = 10.0F;
        m_Azimuth = 0.0F;
        m_Polar = 0.785F; // 45 degrees
    }

    UpdateProjectionMatrix();
    Log::Debug("Camera: Reset to default position (ortho={})", m_UseOrthographic);
}

void Camera::SetOrthographic(float32_t orthoSize)
{
    m_UseOrthographic = true;
    m_OrthoSize = orthoSize;
    UpdateProjectionMatrix();
    Log::Debug("Camera: Set to orthographic mode (size={})", orthoSize);
}

void Camera::SetPerspective(float32_t fovDegrees)
{
    m_UseOrthographic = false;
    m_FieldOfView = fovDegrees;
    UpdateProjectionMatrix();
    Log::Debug("Camera: Set to perspective mode (FOV={}°)", fovDegrees);
}

//=================================================================================================
// Private Functions
//=================================================================================================

void Camera::UpdateProjectionMatrix()
{
    if (m_UseOrthographic)
    {
        // Orthographic projection (like Galaxy-Renderer)
        float32_t half_width = m_OrthoSize * m_AspectRatio;
        float32_t half_height = m_OrthoSize;
        m_ProjectionMatrix = glm::ortho(-half_width, half_width, -half_height, half_height,
                                        -m_OrthoSize, m_OrthoSize);
    }
    else
    {
        // Perspective projection
        m_ProjectionMatrix =
            glm::perspective(glm::radians(m_FieldOfView), m_AspectRatio, m_NearPlane, m_FarPlane);
    }
}

} // namespace Orogena::Render
