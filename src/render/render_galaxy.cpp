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
    m_NumStars = static_cast<int32_t>(stars.size());

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
    m_NumDust = static_cast<int32_t>(dust.size());

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
    m_NumH2 = static_cast<int32_t>(h2.size());

    std::vector<GalaxyParticle> h2Particles;
    h2Particles.reserve(m_NumH2);

    for (const auto& region : h2)
    {
        GalaxyParticle particle;
        particle.position = glm::vec2(region.position.x, region.position.y);
        particle.temperature = static_cast<float32_t>(region.temperature);
        particle.magnitude = static_cast<float32_t>(region.magnitude);
        h2Particles.push_back(particle);
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

    Log::Debug("GalaxyRenderer: Updated from model ({} stars, {} dust, {} H2)", m_NumStars,
               m_NumDust, m_NumH2);
}

void GalaxyRenderer::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    if (!m_Initialized || m_NumStars == 0)
    {
        return;
    }

    SetupRenderState();

    // Render particles in order: dust (back) -> stars -> bright stars (front)
    if (m_DisplayFlags & GALAXY_SHOW_DUST)
    {
        RenderDust(viewMatrix, projectionMatrix);
    }

    if (m_DisplayFlags & GALAXY_SHOW_H2)
    {
        RenderH2Regions(viewMatrix, projectionMatrix);
    }

    if (m_DisplayFlags & GALAXY_SHOW_STARS)
    {
        RenderStars(viewMatrix, projectionMatrix);
        RenderBrightStars(viewMatrix, projectionMatrix);
    }

    RestoreRenderState();
}

//=================================================================================================
// Private Functions
//=================================================================================================

/**************************************************************************************************/
void GalaxyRenderer::RenderStars(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    m_Shader->Bind();
    m_Shader->SetUniform("uView", viewMatrix);
    m_Shader->SetUniform("uProjection", projectionMatrix);
    m_Shader->SetUniform("uPointScale", c_StarPointScale);
    m_Shader->SetUniform("uFoV", 28174.0F); // Will be dynamic later

    // Bind textures
    m_GL->glActiveTexture(GL_TEXTURE0);
    m_GL->glBindTexture(GL_TEXTURE_2D, m_SpectralRenderer->GetPointSpriteTexture());
    m_Shader->SetUniform("uPointTexture", 0);

    m_GL->glActiveTexture(GL_TEXTURE1);
    m_GL->glBindTexture(GL_TEXTURE_1D, m_SpectralRenderer->GetColorRampTexture());
    m_Shader->SetUniform("uColorRamp", 1);

    // Draw all stars
    m_StarVAO->bind();
    m_GL->glDrawArrays(GL_POINTS, 0, m_NumStars);
    m_StarVAO->release();

    m_Shader->Unbind();
}

/**************************************************************************************************/
void GalaxyRenderer::RenderBrightStars(const glm::mat4& viewMatrix,
                                       const glm::mat4& projectionMatrix)
{
    // Render first 1/30th of stars with larger points for emphasis
    const int32_t numBrightStars = m_NumStars / 30;

    if (numBrightStars == 0)
    {
        return;
    }

    m_Shader->Bind();
    m_Shader->SetUniform("uView", viewMatrix);
    m_Shader->SetUniform("uProjection", projectionMatrix);
    m_Shader->SetUniform("uPointScale", c_BrightStarPointScale);
    m_Shader->SetUniform("uFoV", 28174.0F);

    m_GL->glActiveTexture(GL_TEXTURE0);
    m_GL->glBindTexture(GL_TEXTURE_2D, m_SpectralRenderer->GetPointSpriteTexture());
    m_Shader->SetUniform("uPointTexture", 0);

    m_GL->glActiveTexture(GL_TEXTURE1);
    m_GL->glBindTexture(GL_TEXTURE_1D, m_SpectralRenderer->GetColorRampTexture());
    m_Shader->SetUniform("uColorRamp", 1);

    m_StarVAO->bind();
    m_GL->glDrawArrays(GL_POINTS, 0, numBrightStars);
    m_StarVAO->release();

    m_Shader->Unbind();
}

/**************************************************************************************************/
void GalaxyRenderer::RenderDust(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    if (m_NumDust == 0)
    {
        return;
    }

    m_Shader->Bind();
    m_Shader->SetUniform("uView", viewMatrix);
    m_Shader->SetUniform("uProjection", projectionMatrix);
    m_Shader->SetUniform("uPointScale", c_DustPointScale);
    m_Shader->SetUniform("uFoV", 28174.0F);

    m_GL->glActiveTexture(GL_TEXTURE0);
    m_GL->glBindTexture(GL_TEXTURE_2D, m_SpectralRenderer->GetPointSpriteTexture());
    m_Shader->SetUniform("uPointTexture", 0);

    m_GL->glActiveTexture(GL_TEXTURE1);
    m_GL->glBindTexture(GL_TEXTURE_1D, m_SpectralRenderer->GetColorRampTexture());
    m_Shader->SetUniform("uColorRamp", 1);

    m_DustVAO->bind();
    m_GL->glDrawArrays(GL_POINTS, 0, m_NumDust);
    m_DustVAO->release();

    m_Shader->Unbind();
}

/**************************************************************************************************/
void GalaxyRenderer::RenderH2Regions(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    if (m_NumH2 == 0)
    {
        return;
    }

    m_Shader->Bind();
    m_Shader->SetUniform("uView", viewMatrix);
    m_Shader->SetUniform("uProjection", projectionMatrix);
    m_Shader->SetUniform("uPointScale", c_H2PointScale);
    m_Shader->SetUniform("uFoV", 28174.0F);

    m_GL->glActiveTexture(GL_TEXTURE0);
    m_GL->glBindTexture(GL_TEXTURE_2D, m_SpectralRenderer->GetPointSpriteTexture());
    m_Shader->SetUniform("uPointTexture", 0);

    m_GL->glActiveTexture(GL_TEXTURE1);
    m_GL->glBindTexture(GL_TEXTURE_1D, m_SpectralRenderer->GetColorRampTexture());
    m_Shader->SetUniform("uColorRamp", 1);

    m_H2VAO->bind();
    m_GL->glDrawArrays(GL_POINTS, 0, m_NumH2);
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
