/**************************************************************************************************/
/**
 * @file render_galaxy.cpp
 * @brief Implementation of Galaxy
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

#include "render_galaxy.h"

#include <vector>

#include "utils/utils_logger.h"

#include <GL/gl.h>

namespace Orogena::Render
{

//=================================================================================================
// Static Member Initialization
//=================================================================================================

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

/**************************************************************************************************/
GalaxyRenderer::GalaxyRenderer(QOpenGLFunctions_4_5_Core* gl) : m_GL(gl)
{
    Log::Debug("Galaxy: Created");
}

/**************************************************************************************************/
GalaxyRenderer::~GalaxyRenderer()
{
    Log::Debug("GalaxyRenderer: Destroyed");
}

//=================================================================================================
// Public Functions
//=================================================================================================

/**************************************************************************************************/
bool GalaxyRenderer::Initialize()
{
    if (m_Initialized)
    {
        Log::Warn("GalaxyRenderer: Already initialized");
        return true;
    }

    // Create spectral renderer for textures
    m_SpectralRenderer = std::make_unique<SpectralRenderer>(m_GL);
    if (!m_SpectralRenderer->Initialize())
    {
        Log::Error("GalaxyRenderer: Failed to initialize spectral renderer");
        return false;
    }

    // Create shader program
    m_Shader = std::make_unique<Shader>(m_GL);
    if (!m_Shader->LoadFromFiles("shaders/galaxy.vert", "shaders/galaxy.frag"))
    {
        Log::Error("GalaxyRenderer: Failed to load galaxy shaders");
        return false;
    }

    // Create VAO/VBO for stars
    m_StarVAO = std::make_unique<QOpenGLVertexArrayObject>();
    if (!m_StarVAO->create())
    {
        Log::Error("GalaxyRenderer: Failed to create star VAO");
        return false;
    }

    m_StarVBO = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    if (!m_StarVBO->create())
    {
        Log::Error("GalaxyRenderer: Failed to create star VBO");
        return false;
    }

    // Create VAO/VBO for dust
    m_DustVAO = std::make_unique<QOpenGLVertexArrayObject>();
    if (!m_DustVAO->create())
    {
        Log::Error("GalaxyRenderer: Failed to create dust VAO");
        return false;
    }

    m_DustVBO = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    if (!m_DustVBO->create())
    {
        Log::Error("GalaxyRenderer: Failed to create dust VBO");
        return false;
    }

    // Create VAO/VBO for H2 regions
    m_H2VAO = std::make_unique<QOpenGLVertexArrayObject>();
    if (!m_H2VAO->create())
    {
        Log::Error("GalaxyRenderer: Failed to create H2 VAO");
        return false;
    }

    m_H2VBO = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    if (!m_H2VBO->create())
    {
        Log::Error("GalaxyRenderer: Failed to create H2 VBO");
        return false;
    }

    m_Initialized = true;
    Log::Info("GalaxyRenderer: Initialized successfully");
    return true;
}

/**************************************************************************************************/
void GalaxyRenderer::UpdateFromModel(const Orogena::Galaxy::Model& model)
{
    if (!m_Initialized)
    {
        Log::Warn("GalaxyRenderer: Cannot update from model - not initialized");
        return;
    }

    // Convert star data to particle format
    const auto& stars = model.GetStars();
    m_NumStars = stars.size();

    std::vector<GalaxyParticle> starParticles;
    starParticles.reserve(m_NumStars);

    for (const auto& star : stars)
    {
        GalaxyParticle particle;
        particle.position = glm::vec2(star.position.x, star.position.y);
        particle.temperature = static_cast<float32_t>(star.temperature);
        particle.magnitude = static_cast<float32_t>(star.magnitude);
        starParticles.push_back(particle);
    }

    // Upload star data to GPU
    m_StarVAO->bind();
    m_StarVBO->bind();
    m_StarVBO->allocate(starParticles.data(),
                        static_cast<int32_t>(starParticles.size() * sizeof(GalaxyParticle)));

    // Set vertex attributes
    m_GL->glEnableVertexAttribArray(0); // position
    m_GL->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GalaxyParticle),
                                reinterpret_cast<void*>(offsetof(GalaxyParticle, position)));

    m_GL->glEnableVertexAttribArray(1); // temperature
    m_GL->glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(GalaxyParticle),
                                reinterpret_cast<void*>(offsetof(GalaxyParticle, temperature)));

    m_GL->glEnableVertexAttribArray(2); // magnitude
    m_GL->glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GalaxyParticle),
                                reinterpret_cast<void*>(offsetof(GalaxyParticle, magnitude)));

    m_StarVAO->release();

    // Convert dust data
    const auto& dust = model.GetDust();
    m_NumDust = dust.size();

    std::vector<GalaxyParticle> dustParticles;
    dustParticles.reserve(m_NumDust);

    for (const auto& d : dust)
    {
        GalaxyParticle particle;
        particle.position = glm::vec2(d.position.x, d.position.y);
        particle.temperature = static_cast<float32_t>(d.temperature);
        particle.magnitude = static_cast<float32_t>(d.magnitude);
        dustParticles.push_back(particle);
    }

    // Upload dust data
    m_DustVAO->bind();
    m_DustVBO->bind();
    m_DustVBO->allocate(dustParticles.data(),
                        static_cast<int32_t>(dustParticles.size() * sizeof(GalaxyParticle)));

    m_GL->glEnableVertexAttribArray(0);
    m_GL->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GalaxyParticle),
                                reinterpret_cast<void*>(offsetof(GalaxyParticle, position)));
    m_GL->glEnableVertexAttribArray(1);
    m_GL->glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(GalaxyParticle),
                                reinterpret_cast<void*>(offsetof(GalaxyParticle, temperature)));
    m_GL->glEnableVertexAttribArray(2);
    m_GL->glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GalaxyParticle),
                                reinterpret_cast<void*>(offsetof(GalaxyParticle, magnitude)));

    m_DustVAO->release();

    // Convert H2 data
    const auto& h2 = model.GetH2Regions();
    m_NumH2 = h2.size();

    std::vector<GalaxyParticle> h2Particles;
    h2Particles.reserve(m_NumH2);

    // Calculate H2 pair data for distance-based sizing (matches original algorithm)
    m_H2PairData.clear();
    const size_t numPairs = m_NumH2 / 2U;
    m_H2PairData.reserve(numPairs);

    for (uint32_t i = 0; i < numPairs; ++i)
    {
        const uint32_t k1 = i * 2;
        const uint32_t k2 = i * 2 + 1;

        const auto& region1 = h2[k1];
        const auto& region2 = h2[k2];

        // Convert to particles for VBO
        GalaxyParticle particle1;
        particle1.position = glm::vec2(region1.position.x, region1.position.y);
        particle1.temperature = static_cast<float32_t>(region1.temperature);
        particle1.magnitude = static_cast<float32_t>(region1.magnitude);
        h2Particles.push_back(particle1);

        GalaxyParticle particle2;
        particle2.position = glm::vec2(region2.position.x, region2.position.y);
        particle2.temperature = static_cast<float32_t>(region2.temperature);
        particle2.magnitude = static_cast<float32_t>(region2.magnitude);
        h2Particles.push_back(particle2);

        // Calculate distance and size for this pair (matching original algorithm)
        const glm::vec2 pos1(region1.position.x, region1.position.y);
        const glm::vec2 pos2(region2.position.x, region2.position.y);
        const float32_t distance = glm::length(pos2 - pos1);

        // Original formula: size = ((1000 - distance) / 10) - 50
        const float32_t size = ((1000.0F - distance) / 10.0F) - 50.0F;

        // Store pair data for rendering
        GalaxyRenderer::H2PairData pairData;
        pairData.position1 = pos1;
        pairData.position2 = pos2;
        pairData.size = std::max(size, 0.0F); // Clamp to non-negative
        m_H2PairData.push_back(pairData);
    }

    // Upload H2 data
    m_H2VAO->bind();
    m_H2VBO->bind();
    m_H2VBO->allocate(h2Particles.data(),
                      static_cast<int32_t>(h2Particles.size() * sizeof(GalaxyParticle)));

    m_GL->glEnableVertexAttribArray(0);
    m_GL->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GalaxyParticle),
                                reinterpret_cast<void*>(offsetof(GalaxyParticle, position)));
    m_GL->glEnableVertexAttribArray(1);
    m_GL->glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(GalaxyParticle),
                                reinterpret_cast<void*>(offsetof(GalaxyParticle, temperature)));
    m_GL->glEnableVertexAttribArray(2);
    m_GL->glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GalaxyParticle),
                                reinterpret_cast<void*>(offsetof(GalaxyParticle, magnitude)));

    m_H2VAO->release();
}

void GalaxyRenderer::Render(const glm::mat4& viewMatrix,
                            const glm::mat4& projectionMatrix,
                            float32_t        fov)
{
    if (!m_Initialized || m_NumStars == 0)
    {
        return;
    }

    SetupRenderState();

    // Render particles in order: dust (back) -> stars -> bright stars (front)
    if (m_DisplayFlags & GALAXY_SHOW_DUST)
    {
        RenderDust(viewMatrix, projectionMatrix, fov);
    }

    if (m_DisplayFlags & GALAXY_SHOW_H2)
    {
        RenderH2Regions(viewMatrix, projectionMatrix, fov);
    }

    if (m_DisplayFlags & GALAXY_SHOW_STARS)
    {
        RenderStars(viewMatrix, projectionMatrix, fov);
        RenderBrightStars(viewMatrix, projectionMatrix, fov);
    }

    RestoreRenderState();
}

//=================================================================================================
// Private Functions
//=================================================================================================

/**************************************************************************************************/
void GalaxyRenderer::RenderStars(const glm::mat4& viewMatrix,
                                 const glm::mat4& projectionMatrix,
                                 float32_t        fov)
{
    m_Shader->Bind();
    m_Shader->SetUniform("uView", viewMatrix);
    m_Shader->SetUniform("uProjection", projectionMatrix);
    m_Shader->SetUniform("uPointScale", c_StarPointScale); // Fixed size: 3 pixels
    m_Shader->SetUniform("uFoV", fov);
    m_Shader->SetUniform("uBrightBoost", 0.0F);                            // Normal stars, no boost
    m_Shader->SetUniform("uColorMultiplier", glm::vec3(1.0F, 1.0F, 1.0F)); // No color shift

    // Bind textures
    m_GL->glActiveTexture(GL_TEXTURE0);
    m_GL->glBindTexture(GL_TEXTURE_2D, m_SpectralRenderer->GetPointSpriteTexture());
    m_Shader->SetUniform("uPointTexture", 0);

    m_GL->glActiveTexture(GL_TEXTURE1);
    m_GL->glBindTexture(GL_TEXTURE_1D, m_SpectralRenderer->GetColorRampTexture());
    m_Shader->SetUniform("uColorRamp", 1);

    // Draw all stars
    m_StarVAO->bind();
    m_GL->glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(m_NumStars));
    m_StarVAO->release();

    m_Shader->Unbind();
}

/**************************************************************************************************/
void GalaxyRenderer::RenderBrightStars(const glm::mat4& viewMatrix,
                                       const glm::mat4& projectionMatrix,
                                       float32_t        fov)
{
    // Render first 1/30th of stars with larger points for emphasis
    const size_t numBrightStars = m_NumStars / 30;

    if (numBrightStars == 0)
    {
        return;
    }

    m_Shader->Bind();
    m_Shader->SetUniform("uView", viewMatrix);
    m_Shader->SetUniform("uProjection", projectionMatrix);
    m_Shader->SetUniform("uPointScale", c_BrightStarPointScale); // Fixed size: 6 pixels
    m_Shader->SetUniform("uFoV", fov);
    m_Shader->SetUniform("uBrightBoost", 0.2F); // Bright stars get 0.2 boost like original
    m_Shader->SetUniform("uColorMultiplier", glm::vec3(1.0F, 1.0F, 1.0F)); // No color shift

    m_GL->glActiveTexture(GL_TEXTURE0);
    m_GL->glBindTexture(GL_TEXTURE_2D, m_SpectralRenderer->GetPointSpriteTexture());
    m_Shader->SetUniform("uPointTexture", 0);

    m_GL->glActiveTexture(GL_TEXTURE1);
    m_GL->glBindTexture(GL_TEXTURE_1D, m_SpectralRenderer->GetColorRampTexture());
    m_Shader->SetUniform("uColorRamp", 1);

    m_StarVAO->bind();
    m_GL->glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(numBrightStars));
    m_StarVAO->release();

    m_Shader->Unbind();
}

/**************************************************************************************************/
void GalaxyRenderer::RenderDust(const glm::mat4& viewMatrix,
                                const glm::mat4& projectionMatrix,
                                float32_t        fov)
{
    if (m_NumDust == 0)
    {
        return;
    }

    // Query maximum point size supported by hardware
    float32_t maxPointSize = 0.0F;
    m_GL->glGetFloatv(GL_POINT_SIZE_MAX, &maxPointSize);

    // Scale dust point size with FOV to match original: size = 100 * (28174 / fov)
    constexpr float32_t c_ReferenceFOV = 28174.0F;
    const float32_t     dustPointSize =
        std::min(c_DustPointScale * (c_ReferenceFOV / fov), maxPointSize);

    m_Shader->Bind();
    m_Shader->SetUniform("uView", viewMatrix);
    m_Shader->SetUniform("uProjection", projectionMatrix);
    m_Shader->SetUniform("uPointScale", dustPointSize); // Dynamically scaled
    m_Shader->SetUniform("uFoV", fov);
    m_Shader->SetUniform("uBrightBoost", 0.0F); // No brightness boost for dust
    m_Shader->SetUniform("uColorMultiplier", glm::vec3(1.0F, 1.0F, 1.0F)); // No color shift

    m_GL->glActiveTexture(GL_TEXTURE0);
    m_GL->glBindTexture(GL_TEXTURE_2D, m_SpectralRenderer->GetPointSpriteTexture());
    m_Shader->SetUniform("uPointTexture", 0);

    m_GL->glActiveTexture(GL_TEXTURE1);
    m_GL->glBindTexture(GL_TEXTURE_1D, m_SpectralRenderer->GetColorRampTexture());
    m_Shader->SetUniform("uColorRamp", 1);

    m_DustVAO->bind();
    m_GL->glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(m_NumDust));
    m_DustVAO->release();

    m_Shader->Unbind();
}

/**************************************************************************************************/
void GalaxyRenderer::RenderH2Regions(const glm::mat4& viewMatrix,
                                     const glm::mat4& projectionMatrix,
                                     float32_t        fov)
{
    if (m_NumH2 == 0 || m_NumH2 < 2)
    {
        return;
    }

    // H2 regions are rendered as PAIRS with distance-based sizing matching original
    // Original algorithm:
    // - Calculate distance between paired particles (k1, k2)
    // - size = ((1000 - distance) / 10) - 50
    // - Draw twice: large colored nebula + small white center

    m_Shader->Bind();
    m_Shader->SetUniform("uView", viewMatrix);
    m_Shader->SetUniform("uProjection", projectionMatrix);
    m_Shader->SetUniform("uFoV", fov);

    m_GL->glActiveTexture(GL_TEXTURE0);
    m_GL->glBindTexture(GL_TEXTURE_2D, m_SpectralRenderer->GetPointSpriteTexture());
    m_Shader->SetUniform("uPointTexture", 0);

    m_GL->glActiveTexture(GL_TEXTURE1);
    m_GL->glBindTexture(GL_TEXTURE_1D, m_SpectralRenderer->GetColorRampTexture());
    m_Shader->SetUniform("uColorRamp", 1);

    m_H2VAO->bind();

    // Render in pairs using pre-calculated distance-based sizing (matches original)
    for (size_t pairIdx = 0; pairIdx < m_H2PairData.size(); ++pairIdx)
    {
        const auto&   pairData = m_H2PairData[pairIdx];
        const int32_t k1 = static_cast<int32_t>(pairIdx * 2);

        // Skip if size is too small (original: if (size < 1) continue)
        if (pairData.size < 1.0F)
        {
            continue;
        }

        // Draw large colored nebula (2x size with red-shift)
        // Original: glPointSize(2 * size), glColor3f(col.r * mag * 2, col.g * mag * 0.5, ...)
        m_Shader->SetUniform("uPointScale", 2.0F * pairData.size);
        m_Shader->SetUniform("uBrightBoost", 0.0F);
        m_Shader->SetUniform("uColorMultiplier", glm::vec3(2.0F, 0.5F, 0.5F)); // Red-shift
        m_GL->glDrawArrays(GL_POINTS, k1, 1); // Draw first particle of pair

        // Draw small white center (size/6 with pure white)
        // Original: glPointSize(size / 6), glColor3f(1, 1, 1)
        m_Shader->SetUniform("uPointScale", pairData.size / 6.0F);
        m_Shader->SetUniform("uBrightBoost", 1.0F); // Pure white override (shader checks >= 1.0)
        m_Shader->SetUniform("uColorMultiplier", glm::vec3(1.0F, 1.0F, 1.0F)); // Keep normal
        m_GL->glDrawArrays(GL_POINTS, k1, 1); // Draw same particle again as white center
    }

    m_H2VAO->release();
    m_Shader->Unbind();
}

/**************************************************************************************************/
void GalaxyRenderer::SetupRenderState()
{
    // Enable point sprites and blending
    m_GL->glEnable(GL_BLEND);
    m_GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending for glow
    m_GL->glEnable(GL_PROGRAM_POINT_SIZE);   // Let shader control point size
    m_GL->glDisable(GL_DEPTH_TEST);          // 2D rendering, no depth
}

/**************************************************************************************************/
void GalaxyRenderer::RestoreRenderState()
{
    m_GL->glDisable(GL_BLEND);
    m_GL->glEnable(GL_DEPTH_TEST);
    m_GL->glDisable(GL_PROGRAM_POINT_SIZE);
}

} // namespace Orogena::Render
