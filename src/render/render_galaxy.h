/**************************************************************************************************/
/**
 * @file render_galaxy.h
 * @brief Galaxy renderer using point sprites for stars, dust, and H2 regions
 *
 * @details
 * Renders spiral galaxy simulations with temperature-based spectral coloring and soft point
 * sprites. Uses separate buffers for stars, dust, and H2 regions to optimize rendering with
 * different point sizes and blending modes.
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

#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLVertexArrayObject>

#include <glm/glm.hpp>

#include "galaxy/galaxy_model.h"
#include "render_shader.h"
#include "render_spectral.h"
#include "utils/utils_types.h"

namespace Orogena::Render
{

/**
 * @brief Display flags for galaxy rendering features
 */
enum GalaxyDisplayFlags : uint32_t
{
    GALAXY_SHOW_NONE = 0,
    GALAXY_SHOW_STARS = 1 << 0, ///< Render stars
    GALAXY_SHOW_DUST = 1 << 1,  ///< Render dust clouds
    GALAXY_SHOW_H2 = 1 << 2,    ///< Render H2 regions
    GALAXY_SHOW_ALL = GALAXY_SHOW_STARS | GALAXY_SHOW_DUST | GALAXY_SHOW_H2
};

/**
 * @brief Vertex data for galaxy particles (stars, dust, H2)
 *
 * @details
 * Compact 16-byte structure optimized for GPU bandwidth.
 * Layout matches shader attribute locations in galaxy.vert.
 */
struct GalaxyParticle
{
    glm::vec2 position;    ///< 2D position in parsecs (8 bytes)
    float32_t temperature; ///< Temperature in Kelvin (4 bytes)
    float32_t magnitude;   ///< Brightness multiplier 0-1 (4 bytes)
};

/**
 * @brief Galaxy renderer using modern OpenGL point sprites
 *
 * @details
 * Renders galaxy simulations with three particle types:
 * - **Stars**: Small points (3-6 pixels) with spectral coloring
 * - **Dust**: Large soft sprites (40-100 pixels) forming nebulase
 * - **H2 Regions**: Variable-size regions with reddish tint
 *
 * Uses additive blending for realistic glow effects and temperature-based coloring via 1D texture
 * lookup.
 *
 * **Usage:**
 * @code
 * auto renderer = std::make_unique<Galaxy>(gl);
 * renderer->Initialize();
 * renderer->UpdateFromModel(galaxyModel);
 *
 * // In render loop:
 * renderer->Render(camera.GetViewMatrix(), camera.GetProjectionMatrix());
 * @endcode
 */
class GalaxyRenderer
{
  public:
    //=============================================================================================
    // Public Types
    //=============================================================================================

    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    /**
     * @brief Construct a new Galaxy Renderer
     *
     * @param gl OpenGL function pointers (must remain valid).
     */
    explicit GalaxyRenderer(QOpenGLFunctions_4_5_Core* gl);

    ~GalaxyRenderer();

    // Delete copy operations
    GalaxyRenderer(const GalaxyRenderer&) = delete;
    GalaxyRenderer& operator=(const GalaxyRenderer&) = delete;

    // Delete move operations (OpenGL resources are non-movable)
    GalaxyRenderer(GalaxyRenderer&&) = delete;
    GalaxyRenderer& operator=(GalaxyRenderer&&) = delete;

    //=============================================================================================
    // Public Functions
    //=============================================================================================

    /**
     * @brief Initialize OpenGL resources
     *
     * @return true Initialization succeeded.
     * @return false Initialization failed.
     */
    bool Initialize();

    /**
     * @brief Update GPU buffers from galaxy model data
     *
     * @param model Galaxy model containing star/dust/H2 data.
     *
     * @details
     * Uploads particle data to GPU buffers. Call this when the galaxy
     * model changes (e.g., after simulation step or parameter adjustment).
     */
    void UpdateFromModel(const Orogena::Galaxy::Model& model);

    /**
     * @brief Render the galaxy
     *
     * @param viewMatrix Camera view matrix.
     * @param projectionMatrix Camera projection matrix.
     * @param fov Field of view (orthographic size) for scaling dust particles.
     */
    void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float32_t fov);

    /**
     * @brief Set display flags to show/hide particle types
     *
     * @param flags Bitwise OR of GalaxyDisplayFlags.
     */
    void SetDisplayFlags(uint32_t flags)
    {
        m_DisplayFlags = flags;
    }

    /**
     * @brief Get current display flags
     *
     * @return uint32_t Current flags.
     */
    [[nodiscard]] uint32_t GetDisplayFlags() const
    {
        return m_DisplayFlags;
    }

    /**
     * @brief Check if renderer is initialized
     *
     * @return true Ready to render.
     * @return false Not initialized.
     */
    [[nodiscard]] bool IsInitialized() const
    {
        return m_Initialized;
    }

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

    /**
     * @brief Render stars with small point sprites
     */
    void RenderStars(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float32_t fov);

    /**
     * @brief Render bright stars with larger point sprites
     */
    void RenderBrightStars(const glm::mat4& viewMatrix,
                           const glm::mat4& projectionMatrix,
                           float32_t        fov);

    /**
     * @brief Render dust clouds with large soft sprites
     */
    void RenderDust(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float32_t fov);

    /**
     * @brief Render H2 regions with variable-size sprites
     */
    void
    RenderH2Regions(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float32_t fov);

    /**
     * @brief Setup OpenGL state for point sprite rendering
     */
    void SetupRenderState();

    /**
     * @brief Restore OpenGL state after rendering
     */
    void RestoreRenderState();

    //=============================================================================================
    // Private Members
    //=============================================================================================

    QOpenGLFunctions_4_5_Core* m_GL{nullptr}; ///< OpenGL function pointers
    bool                       m_Initialized{false};

    // Rendering components
    std::unique_ptr<Shader>           m_Shader;           ///< Point sprite shader
    std::unique_ptr<SpectralRenderer> m_SpectralRenderer; ///< Texture generator

    // OpenGL resources - Stars
    std::unique_ptr<QOpenGLVertexArrayObject> m_StarVAO;
    std::unique_ptr<QOpenGLBuffer>            m_StarVBO;
    size_t                                    m_NumStars{0};

    // OpenGL resources - Dust
    std::unique_ptr<QOpenGLVertexArrayObject> m_DustVAO;
    std::unique_ptr<QOpenGLBuffer>            m_DustVBO;
    size_t                                    m_NumDust{0};

    // OpenGL resources - H2 Regions
    std::unique_ptr<QOpenGLVertexArrayObject> m_H2VAO;
    std::unique_ptr<QOpenGLBuffer>            m_H2VBO;
    size_t                                    m_NumH2{0};

    // H2 pair data for distance-based sizing (matches original algorithm)
    struct H2PairData
    {
        glm::vec2 position1; ///< First particle position
        glm::vec2 position2; ///< Second particle position
        float32_t size;      ///< Calculated size from distance
    };
    std::vector<H2PairData> m_H2PairData; ///< Cached pair data for rendering

    // Display settings
    uint32_t m_DisplayFlags{GALAXY_SHOW_ALL};

    // Point size parameters matching original Galaxy-Renderer
    static constexpr float32_t c_StarPointScale = 3.0F;       ///< Normal stars (matches original)
    static constexpr float32_t c_BrightStarPointScale = 6.0F; ///< Bright stars (matches original)
    static constexpr float32_t c_DustPointScale =
        100.0F;                                        ///< Dust clouds (base size, scaled by FOV)
    static constexpr float32_t c_H2PointScale = 60.0F; ///< H2 regions
};

} // namespace Orogena::Render
