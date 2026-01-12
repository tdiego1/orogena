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

#include <QImage>
#include <qopenglfunctions_4_5_core.h>

#include "math/math_interpolation.h"
#include "utils/utils_logger.h"

#include <cmath>

namespace Orogena::Render
{

//=================================================================================================
// Static Member Initialization
//=================================================================================================

// SMPTE blackbody color table matching original Galaxy-Renderer
// Source: specrend.cpp by John Walker (public domain)
// Range: 1000K - 10000K to match original implementation
static constexpr struct ColorEntry
{
    float32_t        temperatureK;
    Utils::ColorRGBF color;
} c_ColorTable[] = {
    // SMPTE normalized RGB values from specrend.cpp bb_spectrum output
    {1000.0F, {1.000F, 0.007F, 0.000F}},  // Deep red
    {1500.0F, {1.000F, 0.126F, 0.000F}},
    {2000.0F, {1.000F, 0.234F, 0.010F}},
    {2500.0F, {1.000F, 0.349F, 0.067F}},
    {3000.0F, {1.000F, 0.454F, 0.151F}},
    {3500.0F, {1.000F, 0.549F, 0.254F}},
    {4000.0F, {1.000F, 0.635F, 0.370F}},
    {4500.0F, {1.000F, 0.710F, 0.493F}},
    {5000.0F, {1.000F, 0.778F, 0.620F}},
    {5500.0F, {1.000F, 0.837F, 0.746F}},
    {6000.0F, {1.000F, 0.890F, 0.869F}},  // Near white
    {6500.0F, {1.000F, 0.937F, 0.988F}},
    {7000.0F, {0.907F, 0.888F, 1.000F}},  // Transition to blue
    {7500.0F, {0.827F, 0.839F, 1.000F}},
    {8000.0F, {0.762F, 0.800F, 1.000F}},
    {8500.0F, {0.711F, 0.766F, 1.000F}},
    {9000.0F, {0.668F, 0.738F, 1.000F}},
    {9500.0F, {0.632F, 0.714F, 1.000F}},
    {10000.0F, {0.602F, 0.693F, 1.000F}}, // Blue
};

static constexpr int32_t   c_ColorTableSize = sizeof(c_ColorTable) / sizeof(c_ColorTable[0]);
static constexpr float32_t c_MinTemperature = 1000.0F;
static constexpr float32_t c_MaxTemperature = 10000.0F; // Matches original Galaxy-Renderer
static constexpr int32_t   c_ColorRampSize = 256;  // 1D texture width
static constexpr int32_t   c_PointSpriteSize = 64; // 2D texture dimensions

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

/**************************************************************************************************/
SpectralRenderer::SpectralRenderer(QOpenGLFunctions_4_5_Core* gl) : m_GL(gl)
{
    Log::Debug("Spectral Renderer: Created");
}

/**************************************************************************************************/
SpectralRenderer::~SpectralRenderer()
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
bool SpectralRenderer::Initialize()
{
    GenerateColorRampTexture();
    GeneratePointSpriteTexture();

    Log::Info("Spectral Renderer: Initialized (color ramp={}, point={})", m_ColorRampTexture,
              m_PointSpriteTexture);

    return (m_ColorRampTexture != 0) && (m_PointSpriteTexture != 0);
}

/**************************************************************************************************/
Utils::ColorRGBF SpectralRenderer::TemperatureToColor(float32_t temperatureK)
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
void SpectralRenderer::GenerateColorRampTexture()
{
    // Generate 1D texture data: sample color table at regular intervals
    std::vector<uint8_t> pixels(c_ColorRampSize * 3); // RGB

    for (uint32_t i = 0; i < c_ColorRampSize; ++i)
    {
        // Map texture coordinate [0, 1] to temperature range using Remap
        const float32_t t = static_cast<float32_t>(i) / static_cast<float32_t>(c_ColorRampSize - 1);
        const float64_t temperature = Math::Remap(static_cast<float64_t>(t), 0.0, 1.0,
                                                  static_cast<float64_t>(c_MinTemperature),
                                                  static_cast<float64_t>(c_MaxTemperature));

        // Get color for this temperature
        const Utils::ColorRGBF color = TemperatureToColor(static_cast<float32_t>(temperature));

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
void SpectralRenderer::GeneratePointSpriteTexture()
{
    // Try to load the original particle.bmp texture first
    QImage particleImage("resources/textures/particle.bmp");

    if (!particleImage.isNull())
    {
        // The BMP is grayscale - convert to RGBA with brightness as alpha
        const int32_t width = particleImage.width();
        const int32_t height = particleImage.height();

        std::vector<uint8_t> pixels(width * height * 4); // RGBA

        particleImage = particleImage.convertToFormat(QImage::Format_RGB888);

        for (int32_t y = 0; y < height; ++y)
        {
            for (int32_t x = 0; x < width; ++x)
            {
                QRgb    pixel = particleImage.pixel(x, y);
                int32_t brightness = qGray(pixel); // Convert to grayscale value

                const int32_t idx = (y * width + x) * 4;
                pixels[idx + 0] = 255;                              // R - white
                pixels[idx + 1] = 255;                              // G - white
                pixels[idx + 2] = 255;                              // B - white
                pixels[idx + 3] = static_cast<uint8_t>(brightness); // A - from brightness
            }
        }

        // Create OpenGL texture from converted image
        m_GL->glGenTextures(1, &m_PointSpriteTexture);
        m_GL->glBindTexture(GL_TEXTURE_2D, m_PointSpriteTexture);

        // Upload texture data
        m_GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                           pixels.data());

        // Set texture parameters
        m_GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        m_GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        m_GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        m_GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        m_GL->glBindTexture(GL_TEXTURE_2D, 0);

        Log::Info(
            "SpectralRenderer: Loaded original particle.bmp texture ({}x{}) with brightness->alpha",
            width, height);
        return;
    }

    // Fallback: Generate procedural texture if particle.bmp not found
    Log::Warn("SpectralRenderer: particle.bmp not found, generating procedural texture");

    std::vector<uint8_t> pixels(c_PointSpriteSize * c_PointSpriteSize * 4); // RGBA

    const float32_t center = (c_PointSpriteSize - 1) / 2.0F;

    for (uint32_t y = 0; y < c_PointSpriteSize; ++y)
    {
        for (uint32_t x = 0; x < c_PointSpriteSize; ++x)
        {
            // Calculate normalized distance from center [-1, 1]
            const float32_t dx = (static_cast<float32_t>(x) - center) / center;
            const float32_t dy = (static_cast<float32_t>(y) - center) / center;
            const float32_t r2 = dx * dx + dy * dy;

            // Dual-zone falloff: bright core + soft glow (like original particle.bmp)
            // Creates sharp star centers with soft halos
            float32_t alpha;
            if (r2 < 0.09F) // Core: r < 0.3 (0.3² = 0.09)
            {
                alpha = 1.0F; // Full brightness in core
            }
            else
            {
                // Soft exponential falloff from edge of core
                alpha = std::exp(-(r2 - 0.09F) * 5.0F);
            }

            const uint32_t idx = (y * c_PointSpriteSize + x) * 4;
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
