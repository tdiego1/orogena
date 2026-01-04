/**************************************************************************************************/
/**
 * @file render_sphere.h
 * @brief UV sphere rendering for planet visualization
 *
 * @details
 * Generates a parametric UV sphere with configurable subdivision levels.
 * Used as the foundation for rendering planets and celestial bodies.
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

#include "render_mesh.h"
#include "utils/utils_types.h"

namespace Orogena::Render
{

/**************************************************************************************************/
/**
 * @brief UV sphere renderer for planets and celestial bodies
 *
 * @details
 * Generates a parametric UV sphere using latitude/longitude subdivision:
 * - Latitude ranges from 0 (north pole) to π (south pole)
 * - Longitude ranges from 0 to 2π
 * - Normals are normalized position vectors (for unit sphere)
 * - UVs are u = longitude/(2π), v = latitude/π
 *
 * Default configuration: 64 latitude segments × 128 longitude segments
 */
class Sphere : public Mesh
{
  public:
    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    /**
     * @brief Construct a new Sphere renderer
     *
     * @param gl OpenGL function pointers.
     * @param radius Sphere radius in world units (default: 1.0).
     * @param latSegments Number of latitude divisions (default: 64).
     * @param lonSegments Number of longitude divisions (default: 128).
     */
    explicit Sphere(QOpenGLFunctions_4_5_Core* gl,
                    float32_t                  radius = 1.0F,
                    uint32_t                   latSegments = 64,
                    uint32_t                   lonSegments = 128);

    ~Sphere() override;

    // Delete copy operations
    Sphere(const Sphere&) = delete;
    Sphere& operator=(const Sphere&) = delete;

    // Delete move operations
    Sphere(Sphere&&) = delete;
    Sphere& operator=(Sphere&&) = delete;

    //=============================================================================================
    // Public Functions
    //=============================================================================================

    /**
     * @brief Set sphere radius
     *
     * @param radius New radius in world units.
     */
    void SetRadius(float32_t radius);

    /**
     * @brief Set subdivision levels
     *
     * @param latSegments Number of latitude divisions.
     * @param lonSegments Number of longitude divisions.
     */
    void SetSubdivision(uint32_t latSegments, uint32_t lonSegments);

    /**
     * @brief Get sphere radius
     *
     * @return float32_t Current radius.
     */
    [[nodiscard]] float32_t GetRadius() const
    {
        return m_Radius;
    }

    /**
     * @brief Get latitude subdivision count
     *
     * @return uint32_t Number of latitude segments.
     */
    [[nodiscard]] uint32_t GetLatSegments() const
    {
        return m_LatSegments;
    }

    /**
     * @brief Get longitude subdivision count
     *
     * @return uint32_t Number of longitude segments.
     */
    [[nodiscard]] uint32_t GetLonSegments() const
    {
        return m_LonSegments;
    }

  protected:
    //=============================================================================================
    // Protected Functions
    //=============================================================================================

    /**
     * @brief Generate UV sphere geometry
     *
     * @param vertices Output vector for vertex data.
     * @param indices Output vector for index data.
     */
    void GenerateGeometry(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) override;

  private:
    //=============================================================================================
    // Private Members
    //=============================================================================================

    float32_t m_Radius{1.0F};     ///< Sphere radius in world units
    uint32_t  m_LatSegments{64};  ///< Number of latitude divisions
    uint32_t  m_LonSegments{128}; ///< Number of longitude divisions
};

} // namespace Orogena::Render
