/**************************************************************************************************/
/**
 * @file render_grid.h
 * @brief Grid rendering for viewport reference
 *
 * @details
 * Renders a reference grid on the ground plane to aid spatial navigation.
 * Uses simple line rendering with configurable size and spacing.
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */
/**************************************************************************************************/

#pragma once

#include <memory>

#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <glm/glm.hpp>

#include "utils/utils_types.h"

namespace Orogena::Render
{

/**************************************************************************************************/
/**
 * @brief Grid renderer for viewport reference
 *
 * @details
 * Renders a planar grid at Y=0 with configurable size and line spacing.
 * Uses a simple vertex shader and solid color fragment shader.
 */
class Grid
{
  public:
    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    /**
     * @brief Construct a new Grid renderer
     *
     * @param gl OpenGL function pointers.
     */
    explicit Grid(QOpenGLFunctions_4_5_Core* gl);

    ~Grid();

    // Delete copy operations
    Grid(const Grid&) = delete;
    Grid& operator=(const Grid&) = delete;

    // Delete move operations (OpenGL resources are non-movable)
    Grid(Grid&&) = delete;
    Grid& operator=(Grid&&) = delete;

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
     * @brief Render the grid
     *
     * @param viewMatrix View matrix from camera.
     * @param projectionMatrix Projection matrix from camera.
     */
    void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    /**
     * @brief Set grid size and spacing
     *
     * @param size Total grid size in world units.
     * @param spacing Distance between grid lines.
     */
    void SetGridParameters(float32_t size, float32_t spacing);

    /**
     * @brief Set grid line color
     *
     * @param color RGB color for grid lines.
     */
    void SetColor(const glm::vec3& color)
    {
        m_GridColor = color;
    }

    /**
     * @brief Check if grid is initialized
     *
     * @return true Grid is ready to render.
     * @return false Grid is not initialized.
     */
    [[nodiscard]] bool IsInitialized() const
    {
        return m_Initialized;
    }

  private:
    //=============================================================================================
    // Private Functions
    //=============================================================================================

    /**
     * @brief Generate grid line vertices
     */
    void GenerateGridGeometry();

    //=============================================================================================
    // Private Members
    //=============================================================================================

    QOpenGLFunctions_4_5_Core* m_GL{nullptr};        ///< OpenGL function pointers
    bool                       m_Initialized{false}; ///< Initialization state

    // Grid parameters
    float32_t m_GridSize{20.0F};             ///< Total grid size in world units
    float32_t m_GridSpacing{1.0F};           ///< Distance between grid lines
    glm::vec3 m_GridColor{0.5F, 0.5F, 0.5F}; ///< Grid line color (gray)

    // OpenGL resources
    std::unique_ptr<QOpenGLShaderProgram>     m_ShaderProgram;
    std::unique_ptr<QOpenGLVertexArrayObject> m_VAO;
    std::unique_ptr<QOpenGLBuffer>            m_VBO;

    int32_t m_VertexCount{0}; ///< Number of vertices to render
};

} // namespace Orogena::Render
