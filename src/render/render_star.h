/**************************************************************************************************/
/**
 * @file render_star.h
 * @brief Star renderer using layered radial rendering with animations
 *
 * @details
 * Renders individual stars with realistic visuals using multiple layers:
 * - Corona/Atmosphere: Large, soft glow with spectral color
 * - Photosphere: Main star body with surface detail
 * - Specular Highlight: Bright center point for glare
 *
 * Includes animations:
 * - Stellar pulsation (breathing effect)
 * - Rotation (surface pattern movement)
 * - Corona shimmer (atmospheric variation)
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
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

#include <glm/glm.hpp>

#include "render_shader.h"
#include "stellar/stellar_star.h"
#include "utils/utils_types.h"

namespace Orogena::Render
{

/**
 * @brief Star renderer using layered radial sprites with animations
 *
 * @details
 * Renders stars with physically-based visuals using multiple layers:
 *
 * **Rendering Layers** (back to front):
 * 1. Corona/Atmosphere - Large soft glow (2.5-3× radius)
 * 2. Photosphere - Main star body with surface detail
 * 3. Specular Highlight - Bright center point
 *
 * **Animations**:
 * - **Pulsation**: Periodic size variation (varies by spectral type)
 * - **Rotation**: Surface pattern rotation based on stellar rotation period
 * - **Corona Shimmer**: Atmospheric opacity variation
 *
 * **Spectral Accuracy**:
 * Uses star temperature to determine color via blackbody approximation
 *
 * **Usage:**
 * @code
 * auto renderer = std::make_unique<StarRenderer>(gl);
 * renderer->Initialize();
 * renderer->SetStar(star);
 *
 * // In render loop:
 * renderer->Update(deltaTime);
 * renderer->Render(camera.GetViewMatrix(), camera.GetProjectionMatrix());
 * @endcode
 */
class StarRenderer
{
  public:
    //=============================================================================================
    // Public Types
    //=============================================================================================

    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    /**
     * @brief Construct a new Star Renderer
     *
     * @param gl OpenGL function pointers (must remain valid)
     */
    explicit StarRenderer(QOpenGLFunctions_4_5_Core* gl);

    ~StarRenderer();

    // Delete copy operations
    StarRenderer(const StarRenderer&) = delete;
    StarRenderer& operator=(const StarRenderer&) = delete;

    // Delete move operations (OpenGL resources are non-movable)
    StarRenderer(StarRenderer&&) = delete;
    StarRenderer& operator=(StarRenderer&&) = delete;

    //=============================================================================================
    // Public Functions
    //=============================================================================================

    /**
     * @brief Initialize OpenGL resources
     *
     * @return true Initialization succeeded
     * @return false Initialization failed
     */
    bool Initialize();

    /**
     * @brief Set the star to render
     *
     * @param star Star object with properties
     *
     * @details
     * Updates rendering parameters based on star properties (temperature, radius, spectral type).
     * Resets animation state.
     */
    void SetStar(const Stellar::Star& star);

    /**
     * @brief Update animation state
     *
     * @param deltaTime Time elapsed since last update (seconds)
     *
     * @details
     * Updates pulsation, rotation, and corona shimmer animations.
     * Call this once per frame before Render().
     */
    void Update(float32_t deltaTime);

    /**
     * @brief Render the star
     *
     * @param viewMatrix Camera view matrix
     * @param projectionMatrix Camera projection matrix
     *
     * @details
     * Renders all star layers with current animation state.
     * Uses additive blending for realistic glow.
     */
    void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    /**
     * @brief Set animation speed multiplier
     *
     * @param speedMultiplier Animation speed (1.0 = normal, 2.0 = double speed, 0.0 = paused)
     */
    void SetAnimationSpeed(float32_t speedMultiplier)
    {
        m_AnimationSpeed = speedMultiplier;
    }

    /**
     * @brief Get current animation speed
     *
     * @return float32_t Animation speed multiplier
     */
    [[nodiscard]] float32_t GetAnimationSpeed() const
    {
        return m_AnimationSpeed;
    }

    /**
     * @brief Reset animation state to time zero
     */
    void ResetAnimation()
    {
        m_AnimationTime = 0.0F;
    }

    /**
     * @brief Check if renderer is initialized
     *
     * @return true Ready to render
     * @return false Not initialized
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

    /**
     * @brief Animation parameters calculated from star properties
     */
    struct AnimationParams
    {
        float32_t pulsationFrequency;  ///< Pulsation speed (Hz)
        float32_t pulsationAmplitude;  ///< Pulsation magnitude (0-1)
        float32_t rotationSpeed;       ///< Rotation speed (radians/second)
        float32_t shimmerFrequency;    ///< Corona shimmer speed (Hz)
        float32_t shimmerAmplitude;    ///< Corona shimmer magnitude (0-1)
        bool      hasFlares;           ///< Whether star produces flares (M-dwarfs)
    };

    //=============================================================================================
    // Private Functions
    //=============================================================================================

    /**
     * @brief Calculate animation parameters from star properties
     *
     * @param star Star object
     * @return AnimationParams Animation parameters
     */
    AnimationParams CalculateAnimationParams(const Stellar::Star& star);

    /**
     * @brief Render corona layer (outermost glow)
     *
     * @param viewMatrix Camera view matrix
     * @param projectionMatrix Camera projection matrix
     */
    void RenderCorona(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    /**
     * @brief Render photosphere layer (main star body)
     *
     * @param viewMatrix Camera view matrix
     * @param projectionMatrix Camera projection matrix
     */
    void RenderPhotosphere(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    /**
     * @brief Render specular highlight (center glare)
     *
     * @param viewMatrix Camera view matrix
     * @param projectionMatrix Camera projection matrix
     */
    void RenderSpecular(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    /**
     * @brief Setup OpenGL state for star rendering
     */
    void SetupRenderState();

    /**
     * @brief Restore OpenGL state after rendering
     */
    void RestoreRenderState();

    /**
     * @brief Generate radial gradient texture for point sprites
     *
     * @return GLuint OpenGL texture ID
     */
    GLuint GenerateRadialGradientTexture();

    //=============================================================================================
    // Private Members
    //=============================================================================================

    QOpenGLFunctions_4_5_Core* m_GL{nullptr}; ///< OpenGL function pointers
    bool                       m_Initialized{false};

    // Star properties
    float32_t        m_StarRadiusRsol{1.0F};       ///< Star radius in solar radii
    float32_t        m_StarTemperatureK{5772.0F};  ///< Star temperature in Kelvin
    Utils::ColorRGBF m_StarColor{1.0F, 1.0F, 1.0F}; ///< Star RGB color from temperature
    Stellar::Star::SpectralType m_SpectralType{Stellar::Star::SpectralType::G}; ///< Spectral type

    // Animation state
    float32_t        m_AnimationTime{0.0F};   ///< Current animation time (seconds)
    float32_t        m_AnimationSpeed{1.0F};  ///< Animation speed multiplier
    AnimationParams  m_AnimParams;            ///< Current animation parameters

    // Rendering components
    std::unique_ptr<Shader> m_Shader; ///< Star shader program

    // OpenGL resources
    std::unique_ptr<QOpenGLVertexArrayObject> m_VAO;
    std::unique_ptr<QOpenGLBuffer>            m_VBO;
    GLuint                                    m_RadialTexture{0}; ///< Radial gradient texture

    // Rendering constants
    static constexpr float32_t c_CoronaRadiusMultiplier = 2.5F;   ///< Corona size vs star radius
    static constexpr float32_t c_PhotosphereRadiusMultiplier = 1.0F; ///< Photosphere size
    static constexpr float32_t c_SpecularRadiusMultiplier = 0.15F;   ///< Specular size

    static constexpr float32_t c_BaseStarSize = 1600.0F; ///< Base star size in pixels
};

} // namespace Orogena::Render
