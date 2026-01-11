/**************************************************************************************************/
/**
 * @file render_spectral.cpp
 * @brief Implementation of Spectral
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

#include "render_spectral.h"

#include <algorithm>
#include <vector>

#include <qopenglfunctions_4_5_core.h>

#include "math/math_interpolation.h"
#include "utils/utils_logger.h"

#include <cmath>

namespace Orogena::Render
{

//=================================================================================================
// Static Member Initialization
//=================================================================================================

// Blackbody color table based on Mitchell Charity's work
// Source: http://www.vendian.org/mncharity/dir3/blackbody/
static constexpr struct ColorEntry
{
    float32_t        temperatureK;
    Utils::ColorRGBF color;
} c_ColorTable[] = {
    // Red dwarfs (M-type)
    {1000.0F, {1.0000F, 0.0402F, 0.0000F}},
    {1500.0F, {1.0000F, 0.1801F, 0.0000F}},
    {2000.0F, {1.0000F, 0.2996F, 0.0041F}},
    {2500.0F, {1.0000F, 0.4317F, 0.0735F}},
    {3000.0F, {1.0000F, 0.5512F, 0.1765F}},
    {3500.0F, {1.0000F, 0.6575F, 0.3042F}},

    // Orange/red dwarfs (K-type)
    {4000.0F, {1.0000F, 0.7512F, 0.4481F}},
    {4500.0F, {1.0000F, 0.8331F, 0.6005F}},

    // Yellow stars (G-type, Sun-like)
    {5000.0F, {1.0000F, 0.9046F, 0.7559F}},
    {5500.0F, {1.0000F, 0.9668F, 0.9098F}},
    {5772.0F, {1.0000F, 0.9180F, 0.8400F}}, // Sun (exact)
    {6000.0F, {0.9438F, 0.9636F, 1.0000F}},
    {6500.0F, {0.8313F, 0.8878F, 1.0000F}},

    // White stars (F-type)
    {7000.0F, {0.7467F, 0.8283F, 1.0000F}},
    {7500.0F, {0.6814F, 0.7804F, 1.0000F}},

    // Blue-white stars (A-type)
    {8000.0F, {0.6297F, 0.7412F, 1.0000F}},
    {8500.0F, {0.5880F, 0.7085F, 1.0000F}},
    {9000.0F, {0.5538F, 0.6809F, 1.0000F}},
    {9500.0F, {0.5253F, 0.6574F, 1.0000F}},
    {10000.0F, {0.5013F, 0.6372F, 1.0000F}},

    // Blue stars (B-type)
    {12000.0F, {0.4344F, 0.5784F, 1.0000F}},
    {15000.0F, {0.3798F, 0.5273F, 1.0000F}},

    // Hot blue stars (O-type)
    {20000.0F, {0.3356F, 0.4833F, 1.0000F}},
    {30000.0F, {0.3000F, 0.4459F, 1.0000F}},
    {40000.0F, {0.2850F, 0.4296F, 1.0000F}},
};

static constexpr int32_t   c_ColorTableSize = sizeof(c_ColorTable) / sizeof(c_ColorTable[0]);
static constexpr float32_t c_MinTemperature = 1000.0F;
static constexpr float32_t c_MaxTemperature = 40000.0F;
static constexpr int32_t   c_ColorRampSize = 256;  // 1D texture width
static constexpr int32_t   c_PointSpriteSize = 64; // 2D texture dimensions

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

/**************************************************************************************************/
Spectral::Spectral(QOpenGLFunctions_4_5_Core* gl) : m_GL(gl)
{
    Log::Debug("Spectral Renderer: Created");
}

/**************************************************************************************************/
Spectral::~Spectral()
{
    if (m_ColorRampTexture != 0)
    {
        m_GL->glDeleteTextures(1, &m_ColorRampTexture);
    }
    if (m_PointSpriteTexture != 0)
    {
        m_GL->glDeleteTextures(1, &m_PointSpriteTexture);
    }
    Log::Debug("Spectral Renderer: Destroyed");
}

//=================================================================================================
// Public Functions
//=================================================================================================

/**************************************************************************************************/
bool Spectral::Initialize()
{
    GenerateColorRampTexture();
    GeneratePointSpriteTexture();

    Log::Info("Spectral Renderer: Initialized (color ramp={}, point={})", m_ColorRampTexture,
              m_PointSpriteTexture);

    return (m_ColorRampTexture != 0) && (m_PointSpriteTexture != 0);
}

/**************************************************************************************************/
Utils::ColorRGBF Spectral::TemperatureToColor(float32_t temperatureK)
{
    // Clamp to valid range
    temperatureK = Math::Clamp(temperatureK, c_ColorTable[0].temperatureK,
                               c_ColorTable[c_ColorTableSize - 1].temperatureK);

    // Find bracketing entries for interpolation
    for (int32_t i = 0; i < c_ColorTableSize - 1; ++i)
    {
        if (temperatureK >= c_ColorTable[i].temperatureK &&
            temperatureK <= c_ColorTable[i + 1].temperatureK)
        {
            // Calculate interpolation parameter
            const float32_t t =
                Math::InverseLerp(static_cast<float64_t>(temperatureK),
                                  static_cast<float64_t>(c_ColorTable[i].temperatureK),
                                  static_cast<float64_t>(c_ColorTable[i + 1].temperatureK));

            // Interpolate RGB components using Math::Lerp
            Utils::ColorRGBF color;
            color.r = Math::Lerp(t, c_ColorTable[i].color.r, c_ColorTable[i + 1].color.r);
            color.g = Math::Lerp(t, c_ColorTable[i].color.g, c_ColorTable[i + 1].color.g);
            color.b = Math::Lerp(t, c_ColorTable[i].color.b, c_ColorTable[i + 1].color.b);

            return color;
        }
    }

    // Fallback (should never reach here due to clamping)
    return c_ColorTable[c_ColorTableSize / 2].color;
}

//=================================================================================================
// Private Functions
//=================================================================================================

/**************************************************************************************************/
void Spectral::GenerateColorRampTexture()
{
    // Generate 1D texture data: sample color table at regular intervals
    std::vector<uint8_t> pixels(c_ColorRampSize * 3); // RGB

    for (int32_t i = 0; i < c_ColorRampSize; ++i)
    {
        // Map texture coordinate [0, 1] to temperature range using Remap
        const float32_t t = static_cast<float32_t>(i) / static_cast<float32_t>(c_ColorRampSize - 1);
        const float32_t temperature = Math::Remap(static_cast<float64_t>(t), 0.0, 1.0,
                                                  static_cast<float64_t>(c_MinTemperature),
                                                  static_cast<float64_t>(c_MaxTemperature));

        // Get color for this temperature
        const Utils::ColorRGBF color = TemperatureToColor(temperature);

        // Store as RGB bytes
        pixels[i * 3 + 0] = static_cast<uint8_t>(Math::Clamp01(color.r) * 255.0F);
        pixels[i * 3 + 1] = static_cast<uint8_t>(Math::Clamp01(color.g) * 255.0F);
        pixels[i * 3 + 2] = static_cast<uint8_t>(Math::Clamp01(color.b) * 255.0F);
    }

    // Create OpenGL texture
    m_GL->glGenTextures(1, &m_ColorRampTexture);
    m_GL->glBindTexture(GL_TEXTURE_1D, m_ColorRampTexture);

    // Upload texture data
    m_GL->glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB8, c_ColorRampSize, 0, GL_RGB, GL_UNSIGNED_BYTE,
                       pixels.data());

    // Set texture parameters
    m_GL->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_GL->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_GL->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    m_GL->glBindTexture(GL_TEXTURE_1D, 0);

    Log::Debug("SpectralRenderer: Generated color ramp texture ({}x1)", c_ColorRampSize);
}

/**************************************************************************************************/
void Spectral::GeneratePointSpriteTexture()
{
    // Generate 2D texture with Gaussian soft-glow falloff
    std::vector<uint8_t> pixels(c_PointSpriteSize * c_PointSpriteSize * 4); // RGBA

    const float32_t center = (c_PointSpriteSize - 1) / 2.0F;

    for (int32_t y = 0; y < c_PointSpriteSize; ++y)
    {
        for (int32_t x = 0; x < c_PointSpriteSize; ++x)
        {
            // Calculate normalized distance from center [-1, 1]
            const float32_t dx = (static_cast<float32_t>(x) - center) / center;
            const float32_t dy = (static_cast<float32_t>(y) - center) / center;
            const float32_t r2 = dx * dx + dy * dy;

            // Gaussian falloff: exp(-r² * k)
            const float32_t alpha = std::exp(-r2 * 4.0F);

            const int32_t idx = (y * c_PointSpriteSize + x) * 4;
            pixels[idx + 0] = 255;                                                 // R
            pixels[idx + 1] = 255;                                                 // G
            pixels[idx + 2] = 255;                                                 // B
            pixels[idx + 3] = static_cast<uint8_t>(Math::Clamp01(alpha) * 255.0F); // A
        }
    }

    // Create OpenGL texture
    m_GL->glGenTextures(1, &m_PointSpriteTexture);
    m_GL->glBindTexture(GL_TEXTURE_2D, m_PointSpriteTexture);

    // Upload texture data
    m_GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, c_PointSpriteSize, c_PointSpriteSize, 0, GL_RGBA,
                       GL_UNSIGNED_BYTE, pixels.data());

    // Set texture parameters
    m_GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    m_GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    m_GL->glBindTexture(GL_TEXTURE_2D, 0);

    Log::Debug("SpectralRenderer: Generated point sprite texture ({}x{})", c_PointSpriteSize,
               c_PointSpriteSize);
}
} // namespace Orogena::Render
