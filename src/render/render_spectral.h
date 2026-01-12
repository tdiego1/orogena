/**************************************************************************************************/
/**
 * @file render_spectral.h
 * @brief Utility class for spectral rendering in galaxy renderer
 *
 * @details
 * Provides functions to convert star temperatures to RGB colors, and generates OpenGL textures
 * for color ramps and point sprites used in rendering stars with spectral colors.
 *
 * @author Diego Torres
 * @date 2026
 * @copyright Copyright (C) 2026 Diego Torres
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

#include <QOpenGLFunctions_4_5_Core>
#include <qopenglfunctions_4_5_core.h>

#include "utils/utils_types.h"

namespace Orogena::Render
{

/**
 * @brief Spectral rendering utility class
 *
 * @details
 * Handles temperature to color mapping and texture generation for spectral rendering of stars.
 */
class SpectralRenderer
{
  public:
    //=============================================================================================
    // Public Types
    //=============================================================================================

    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    explicit SpectralRenderer(QOpenGLFunctions_4_5_Core* gl);
    ~SpectralRenderer();

    // Delete copy operations
    SpectralRenderer(const SpectralRenderer&) = delete;
    SpectralRenderer& operator=(const SpectralRenderer&) = delete;

    // Default move operations
    SpectralRenderer(SpectralRenderer&&) = default;
    SpectralRenderer& operator=(SpectralRenderer&&) = default;

    //=============================================================================================
    // Public Functions
    //=============================================================================================

    /**
     * @brief Initialize spectral rendering resources
     * @return True if successful, false otherwise
     */
    bool Initialize();

    /**
     * @brief Get color ramp texture ID
     * @return OpenGL texture ID
     */
    uint32_t GetColorRampTexture() const
    {
        return m_ColorRampTexture;
    }

    /**
     * @brief Get point sprite texture ID
     * @return OpenGL texture ID
     */
    uint32_t GetPointSpriteTexture() const
    {
        return m_PointSpriteTexture;
    }

    /**
     * @brief Convert temperature in Kelvin to RGB color
     * @param temperatureK Temperature in Kelvin
     * @return Corresponding RGB color
     */
    static Utils::ColorRGBF TemperatureToColor(float32_t temperatureK);

    //=============================================================================================
    // Public Members
    //=============================================================================================

  private:
    //=============================================================================================
    // Private Types
    //=============================================================================================

    //=============================================================================================
    // Private Functions
    //=============================================================================================

    void GenerateColorRampTexture();

    void GeneratePointSpriteTexture();

    //=============================================================================================
    // Private Members
    //=============================================================================================

    QOpenGLFunctions_4_5_Core* m_GL{nullptr};
    uint32_t                   m_ColorRampTexture{0};   ///< 1D texture: temperature -> RGB
    uint32_t                   m_PointSpriteTexture{0}; ///< 2D texture: soft glow
};

} // namespace Orogena::Render
