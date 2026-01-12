/**************************************************************************************************/
/**
 * @file render_star.cpp
 * @brief Implementation of StarRenderer
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

#include "render_star.h"

#include <cmath>
#include <vector>

#include "utils/utils_logger.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace Orogena::Render
{

//=================================================================================================
// Static Member Initialization
//=================================================================================================

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

/**************************************************************************************************/
StarRenderer::StarRenderer(QOpenGLFunctions_4_5_Core* gl) : m_GL(gl)
{
    Log::Debug("StarRenderer: Created");
}

/**************************************************************************************************/
StarRenderer::~StarRenderer()
{
    if (m_RadialTexture != 0)
    {
        m_GL->glDeleteTextures(1, &m_RadialTexture);
    }
    Log::Debug("StarRenderer: Destroyed");
}

//=================================================================================================
// Public Functions
//=================================================================================================

/**************************************************************************************************/
bool StarRenderer::Initialize()
{
    if (m_Initialized)
    {
        Log::Warn("StarRenderer: Already initialized");
        return true;
    }

    // Create shader program
    m_Shader = std::make_unique<Shader>(m_GL);
    if (!m_Shader->LoadFromFiles("shaders/star.vert", "shaders/star.frag"))
    {
        Log::Error("StarRenderer: Failed to load star shaders");
        return false;
    }

    // Create VAO/VBO for a single point (star center)
    m_VAO = std::make_unique<QOpenGLVertexArrayObject>();
    if (!m_VAO->create())
    {
        Log::Error("StarRenderer: Failed to create VAO");
        return false;
    }

    m_VBO = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    if (!m_VBO->create())
    {
        Log::Error("StarRenderer: Failed to create VBO");
        return false;
    }

    // Setup single point at origin
    m_VAO->bind();
    m_VBO->bind();

    glm::vec2 origin(0.0F, 0.0F);
    m_VBO->allocate(&origin, sizeof(glm::vec2));

    m_GL->glEnableVertexAttribArray(0);
    m_GL->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

    m_VBO->release();
    m_VAO->release();

    // Generate radial gradient texture
    m_RadialTexture = GenerateRadialGradientTexture();

    m_Initialized = true;
    Log::Info("StarRenderer: Initialized successfully");
    return true;
}

/**************************************************************************************************/
void StarRenderer::SetStar(const Stellar::Star& star)
{
    if (!m_Initialized)
    {
        Log::Warn("StarRenderer: Cannot set star - not initialized");
        return;
    }

    // Store star properties
    m_StarRadiusRsol = star.GetRadius();
    m_StarTemperatureK = star.GetTemperature();
    m_StarColor = star.GetColorRGB();
    m_SpectralType = star.GetSpectralType();

    // Calculate animation parameters based on spectral type
    m_AnimParams = CalculateAnimationParams(star);

    // Reset animation
    m_AnimationTime = 0.0F;

    Log::Debug("StarRenderer: Set star - Spectral Type: {}, Temperature: {}K, Radius: {} Rsol",
               star.GetSpectralTypeString(), m_StarTemperatureK, m_StarRadiusRsol);
}

/**************************************************************************************************/
void StarRenderer::Update(float32_t deltaTime)
{
    if (!m_Initialized)
    {
        return;
    }

    m_AnimationTime += deltaTime * m_AnimationSpeed;
}

/**************************************************************************************************/
void StarRenderer::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    if (!m_Initialized)
    {
        return;
    }

    SetupRenderState();

    // Render layers back to front
    RenderCorona(viewMatrix, projectionMatrix);
    RenderPhotosphere(viewMatrix, projectionMatrix);
    RenderSpecular(viewMatrix, projectionMatrix);

    RestoreRenderState();
}

//=================================================================================================
// Private Functions
//=================================================================================================

/**************************************************************************************************/
StarRenderer::AnimationParams StarRenderer::CalculateAnimationParams(const Stellar::Star& star)
{
    AnimationParams params;

    // Animation parameters vary by spectral type
    switch (star.GetSpectralType())
    {
        case Stellar::Star::SpectralType::O:
        case Stellar::Star::SpectralType::B:
            // Hot blue stars: Fast, energetic pulsation
            params.pulsationFrequency = 0.8F;  // Hz
            params.pulsationAmplitude = 0.08F; // 8% variation
            params.rotationSpeed = 0.4F;       // Fast rotation
            params.shimmerFrequency = 1.2F;
            params.shimmerAmplitude = 0.15F;
            params.hasFlares = false;
            break;

        case Stellar::Star::SpectralType::A:
        case Stellar::Star::SpectralType::F:
            // White stars: Moderate pulsation
            params.pulsationFrequency = 0.5F;
            params.pulsationAmplitude = 0.05F;
            params.rotationSpeed = 0.3F;
            params.shimmerFrequency = 0.8F;
            params.shimmerAmplitude = 0.12F;
            params.hasFlares = false;
            break;

        case Stellar::Star::SpectralType::G:
            // Sun-like stars: Gentle, slow pulsation
            params.pulsationFrequency = 0.3F;
            params.pulsationAmplitude = 0.03F; // Very subtle
            params.rotationSpeed = 0.15F;      // ~25 day rotation period
            params.shimmerFrequency = 0.5F;
            params.shimmerAmplitude = 0.08F;
            params.hasFlares = false;
            break;

        case Stellar::Star::SpectralType::K:
            // Orange stars: Slow pulsation
            params.pulsationFrequency = 0.25F;
            params.pulsationAmplitude = 0.04F;
            params.rotationSpeed = 0.1F;
            params.shimmerFrequency = 0.4F;
            params.shimmerAmplitude = 0.1F;
            params.hasFlares = false;
            break;

        case Stellar::Star::SpectralType::M:
            // Red dwarfs: Irregular flicker (flares)
            params.pulsationFrequency = 0.6F;  // Irregular
            params.pulsationAmplitude = 0.12F; // More pronounced
            params.rotationSpeed = 0.05F;      // Very slow
            params.shimmerFrequency = 1.5F;    // Rapid shimmer
            params.shimmerAmplitude = 0.2F;    // Strong variation
            params.hasFlares = true;
            break;

        default:
            // Default to Sun-like
            params.pulsationFrequency = 0.3F;
            params.pulsationAmplitude = 0.03F;
            params.rotationSpeed = 0.15F;
            params.shimmerFrequency = 0.5F;
            params.shimmerAmplitude = 0.08F;
            params.hasFlares = false;
            break;
    }

    return params;
}

/**************************************************************************************************/
void StarRenderer::RenderCorona(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    // Corona shimmer animation (multiple frequencies for natural look)
    const float32_t shimmer1 = std::sin(m_AnimationTime * m_AnimParams.shimmerFrequency);
    const float32_t shimmer2 = std::sin(m_AnimationTime * m_AnimParams.shimmerFrequency * 1.7F);
    const float32_t shimmer3 = std::sin(m_AnimationTime * m_AnimParams.shimmerFrequency * 2.3F);
    const float32_t shimmerFactor = 1.0F + m_AnimParams.shimmerAmplitude *
                                               (0.5F * shimmer1 + 0.3F * shimmer2 + 0.2F * shimmer3);

    // Pulsation affects corona slightly
    const float32_t pulsation = 1.0F + m_AnimParams.pulsationAmplitude *
                                           std::sin(m_AnimationTime * m_AnimParams.pulsationFrequency *
                                                    2.0F * 3.14159265F);

    // Calculate corona size
    const float32_t coronaSize =
        c_BaseStarSize * c_CoronaRadiusMultiplier * m_StarRadiusRsol * pulsation * shimmerFactor;

    m_Shader->Bind();
    m_Shader->SetUniform("uView", viewMatrix);
    m_Shader->SetUniform("uProjection", projectionMatrix);
    m_Shader->SetUniform("uPointSize", coronaSize);
    m_Shader->SetUniform("uColor", glm::vec4(m_StarColor.r, m_StarColor.g, m_StarColor.b, 1.0F));
    m_Shader->SetUniform("uOpacity", 0.3F); // Transparent glow
    m_Shader->SetUniform("uTime", m_AnimationTime);
    m_Shader->SetUniform("uRotationAngle", 0.0F); // Corona doesn't rotate

    // Bind radial gradient texture
    m_GL->glActiveTexture(GL_TEXTURE0);
    m_GL->glBindTexture(GL_TEXTURE_2D, m_RadialTexture);
    m_Shader->SetUniform("uTexture", 0);

    // Draw corona
    m_VAO->bind();
    m_GL->glDrawArrays(GL_POINTS, 0, 1);
    m_VAO->release();

    m_Shader->Unbind();
}

/**************************************************************************************************/
void StarRenderer::RenderPhotosphere(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    // Pulsation animation
    const float32_t pulsation = 1.0F + m_AnimParams.pulsationAmplitude *
                                           std::sin(m_AnimationTime * m_AnimParams.pulsationFrequency *
                                                    2.0F * 3.14159265F);

    // Rotation animation
    const float32_t rotationAngle = m_AnimationTime * m_AnimParams.rotationSpeed;

    // Calculate photosphere size
    const float32_t photosphereSize =
        c_BaseStarSize * c_PhotosphereRadiusMultiplier * m_StarRadiusRsol * pulsation;

    m_Shader->Bind();
    m_Shader->SetUniform("uView", viewMatrix);
    m_Shader->SetUniform("uProjection", projectionMatrix);
    m_Shader->SetUniform("uPointSize", photosphereSize);
    m_Shader->SetUniform("uColor", glm::vec4(m_StarColor.r, m_StarColor.g, m_StarColor.b, 1.0F));
    m_Shader->SetUniform("uOpacity", 0.95F); // Nearly opaque
    m_Shader->SetUniform("uTime", m_AnimationTime);
    m_Shader->SetUniform("uRotationAngle", rotationAngle);

    m_GL->glActiveTexture(GL_TEXTURE0);
    m_GL->glBindTexture(GL_TEXTURE_2D, m_RadialTexture);
    m_Shader->SetUniform("uTexture", 0);

    m_VAO->bind();
    m_GL->glDrawArrays(GL_POINTS, 0, 1);
    m_VAO->release();

    m_Shader->Unbind();
}

/**************************************************************************************************/
void StarRenderer::RenderSpecular(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    // Specular highlight pulses with star
    const float32_t pulsation = 1.0F + m_AnimParams.pulsationAmplitude *
                                           std::sin(m_AnimationTime * m_AnimParams.pulsationFrequency *
                                                    2.0F * 3.14159265F);

    const float32_t specularSize =
        c_BaseStarSize * c_SpecularRadiusMultiplier * m_StarRadiusRsol * pulsation;

    m_Shader->Bind();
    m_Shader->SetUniform("uView", viewMatrix);
    m_Shader->SetUniform("uProjection", projectionMatrix);
    m_Shader->SetUniform("uPointSize", specularSize);
    m_Shader->SetUniform("uColor", glm::vec4(1.0F, 1.0F, 1.0F, 1.0F)); // Pure white
    m_Shader->SetUniform("uOpacity", 1.0F);                             // Fully opaque
    m_Shader->SetUniform("uTime", m_AnimationTime);
    m_Shader->SetUniform("uRotationAngle", 0.0F);

    m_GL->glActiveTexture(GL_TEXTURE0);
    m_GL->glBindTexture(GL_TEXTURE_2D, m_RadialTexture);
    m_Shader->SetUniform("uTexture", 0);

    m_VAO->bind();
    m_GL->glDrawArrays(GL_POINTS, 0, 1);
    m_VAO->release();

    m_Shader->Unbind();
}

/**************************************************************************************************/
void StarRenderer::SetupRenderState()
{
    m_GL->glEnable(GL_BLEND);
    m_GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending for glow
    m_GL->glEnable(GL_PROGRAM_POINT_SIZE);   // Let shader control point size
    m_GL->glDisable(GL_DEPTH_TEST);          // 2D rendering
}

/**************************************************************************************************/
void StarRenderer::RestoreRenderState()
{
    m_GL->glDisable(GL_BLEND);
    m_GL->glEnable(GL_DEPTH_TEST);
    m_GL->glDisable(GL_PROGRAM_POINT_SIZE);
}

/**************************************************************************************************/
GLuint StarRenderer::GenerateRadialGradientTexture()
{
    // Generate soft radial gradient texture for point sprites
    constexpr int32_t c_TextureSize = 512;
    constexpr int32_t c_TextureChannels = 4; // RGBA

    std::vector<uint8_t> data(c_TextureSize * c_TextureSize * c_TextureChannels);

    const float32_t center = static_cast<float32_t>(c_TextureSize) / 2.0F;

    for (int32_t y = 0; y < c_TextureSize; ++y)
    {
        for (int32_t x = 0; x < c_TextureSize; ++x)
        {
            // Calculate distance from center
            const float32_t dx = static_cast<float32_t>(x) - center;
            const float32_t dy = static_cast<float32_t>(y) - center;
            const float32_t dist = std::sqrt(dx * dx + dy * dy) / center;

            // Smooth falloff: alpha = (1 - dist^2)^3 for soft edges
            float32_t alpha = 1.0F - dist;
            alpha = std::max(0.0F, alpha);
            alpha = alpha * alpha * alpha; // Cubic falloff

            const int32_t idx = (y * c_TextureSize + x) * c_TextureChannels;
            data[idx + 0] = 255; // R
            data[idx + 1] = 255; // G
            data[idx + 2] = 255; // B
            data[idx + 3] = static_cast<uint8_t>(alpha * 255.0F); // A
        }
    }

    // Create OpenGL texture
    GLuint textureID = 0;
    m_GL->glGenTextures(1, &textureID);
    m_GL->glBindTexture(GL_TEXTURE_2D, textureID);

    m_GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, c_TextureSize, c_TextureSize, 0, GL_RGBA,
                       GL_UNSIGNED_BYTE, data.data());

    m_GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    m_GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    m_GL->glBindTexture(GL_TEXTURE_2D, 0);

    Log::Debug("StarRenderer: Generated radial gradient texture (ID: {})", textureID);
    return textureID;
}

} // namespace Orogena::Render
