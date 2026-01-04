/**************************************************************************************************/
/**
 * @file render_sphere.cpp
 * @brief Implementation of Sphere
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

#include "render_sphere.h"

#include <cmath>
#include <numbers>

#include "utils/utils_logger.h"

namespace Orogena::Render
{

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

Sphere::Sphere(QOpenGLFunctions_4_5_Core* gl, float32_t radius, uint32_t latSegments,
               uint32_t lonSegments)
    : Mesh(gl), m_Radius(radius), m_LatSegments(latSegments), m_LonSegments(lonSegments)
{
    Log::Debug("Sphere: Created (radius={}, lat={}, lon={})", radius, latSegments, lonSegments);
}

Sphere::~Sphere()
{
    Log::Debug("Sphere: Destroyed");
}

//=================================================================================================
// Public Functions
//=================================================================================================

void Sphere::SetRadius(float32_t radius)
{
    if (radius <= 0.0F)
    {
        Log::Warn("Sphere: Invalid radius {}, must be > 0", radius);
        return;
    }

    m_Radius = radius;

    if (m_Initialized)
    {
        // Regenerate geometry with new radius
        m_Vertices.clear();
        m_Indices.clear();
        GenerateGeometry(m_Vertices, m_Indices);
        UploadGeometry();
        Log::Debug("Sphere: Radius updated to {}", radius);
    }
}

void Sphere::SetSubdivision(uint32_t latSegments, uint32_t lonSegments)
{
    if (latSegments < 2 || lonSegments < 3)
    {
        Log::Warn("Sphere: Invalid subdivision (lat={}, lon={}), minimum is lat=2, lon=3",
                  latSegments, lonSegments);
        return;
    }

    m_LatSegments = latSegments;
    m_LonSegments = lonSegments;

    if (m_Initialized)
    {
        // Regenerate geometry with new subdivision
        m_Vertices.clear();
        m_Indices.clear();
        GenerateGeometry(m_Vertices, m_Indices);
        UploadGeometry();
        Log::Debug("Sphere: Subdivision updated to lat={}, lon={}", latSegments, lonSegments);
    }
}

//=================================================================================================
// Protected Functions
//=================================================================================================

void Sphere::GenerateGeometry(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    using std::numbers::pi;

    // Reserve space for efficiency
    const size_t expected_vertex_count = (m_LatSegments + 1) * (m_LonSegments + 1);
    const size_t expected_index_count = m_LatSegments * m_LonSegments * 6;
    vertices.reserve(expected_vertex_count);
    indices.reserve(expected_index_count);

    // Generate vertices
    for (uint32_t lat = 0; lat <= m_LatSegments; ++lat)
    {
        // Latitude angle: 0 (north pole) to π (south pole)
        float32_t theta = static_cast<float32_t>(lat) / static_cast<float32_t>(m_LatSegments) *
                          static_cast<float32_t>(pi);
        float32_t sin_theta = std::sin(theta);
        float32_t cos_theta = std::cos(theta);

        for (uint32_t lon = 0; lon <= m_LonSegments; ++lon)
        {
            // Longitude angle: 0 to 2π
            float32_t phi =
                static_cast<float32_t>(lon) / static_cast<float32_t>(m_LonSegments) * 2.0F *
                static_cast<float32_t>(pi);
            float32_t sin_phi = std::sin(phi);
            float32_t cos_phi = std::cos(phi);

            // Position on unit sphere
            glm::vec3 position{sin_theta * cos_phi, cos_theta, sin_theta * sin_phi};

            // Normal = normalized position for unit sphere
            glm::vec3 normal = glm::normalize(position);

            // Scale to actual radius
            position *= m_Radius;

            // UV coordinates
            float32_t u = static_cast<float32_t>(lon) / static_cast<float32_t>(m_LonSegments);
            float32_t v = static_cast<float32_t>(lat) / static_cast<float32_t>(m_LatSegments);

            vertices.push_back(Vertex{position, normal, glm::vec2(u, v)});
        }
    }

    // Generate indices for triangle mesh
    for (uint32_t lat = 0; lat < m_LatSegments; ++lat)
    {
        for (uint32_t lon = 0; lon < m_LonSegments; ++lon)
        {
            // Current quad vertices
            uint32_t first = lat * (m_LonSegments + 1) + lon;
            uint32_t second = first + m_LonSegments + 1;

            // First triangle
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            // Second triangle
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    Log::Debug("Sphere: Generated {} vertices and {} indices", vertices.size(), indices.size());
}

} // namespace Orogena::Render
