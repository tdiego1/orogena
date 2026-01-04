/**************************************************************************************************/
/**
 * @file render_mesh.h
 * @brief Base class for 3D mesh rendering with RAII OpenGL resource management
 *
 * @details
 * Provides common functionality for mesh-based rendering including:
 * - OpenGL buffer management (VAO, VBO, EBO)
 * - Shader program lifecycle
 * - Vertex attribute configuration
 * - Rendering modes (wireframe/solid)
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

#include <memory>
#include <vector>

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
 * @brief Vertex data structure for mesh rendering
 *
 * @details
 * Contains position, normal, and UV coordinates for a single vertex.
 * Tightly packed for efficient GPU transfer.
 */
struct Vertex
{
    glm::vec3 position; ///< Vertex position (x, y, z)
    glm::vec3 normal;   ///< Normal vector for lighting
    glm::vec2 uv;       ///< Texture coordinates (u, v)
};

/**************************************************************************************************/
/**
 * @brief Rendering mode for mesh display
 */
enum class RenderMode
{
    SOLID,     ///< Filled triangles with lighting
    WIREFRAME, ///< Wireframe lines only
};

/**************************************************************************************************/
/**
 * @brief Base class for 3D mesh rendering
 *
 * @details
 * Manages OpenGL resources for mesh rendering using RAII patterns.
 * Derived classes implement geometry generation via GenerateGeometry().
 */
class Mesh
{
  public:
    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    /**
     * @brief Construct a new Mesh renderer
     *
     * @param gl OpenGL function pointers.
     */
    explicit Mesh(QOpenGLFunctions_4_5_Core* gl);

    virtual ~Mesh();

    // Delete copy operations
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Delete move operations (OpenGL resources are non-movable)
    Mesh(Mesh&&) = delete;
    Mesh& operator=(Mesh&&) = delete;

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
     * @brief Render the mesh
     *
     * @param viewMatrix View matrix from camera.
     * @param projectionMatrix Projection matrix from camera.
     */
    void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    /**
     * @brief Set mesh color
     *
     * @param color RGB color for the mesh.
     */
    void SetColor(const glm::vec3& color)
    {
        m_MeshColor = color;
    }

    /**
     * @brief Set mesh position
     *
     * @param position World position.
     */
    void SetPosition(const glm::vec3& position)
    {
        m_Position = position;
    }

    /**
     * @brief Set rendering mode
     *
     * @param mode Rendering mode (solid or wireframe).
     */
    void SetRenderMode(RenderMode mode)
    {
        m_RenderMode = mode;
    }

    /**
     * @brief Check if mesh is initialized
     *
     * @return true Mesh is ready to render.
     * @return false Mesh is not initialized.
     */
    [[nodiscard]] bool IsInitialized() const
    {
        return m_Initialized;
    }

  protected:
    //=============================================================================================
    // Protected Functions
    //=============================================================================================

    /**
     * @brief Generate mesh geometry (implemented by derived classes)
     *
     * @param vertices Output vector for vertex data.
     * @param indices Output vector for index data.
     */
    virtual void GenerateGeometry(std::vector<Vertex>&   vertices,
                                  std::vector<uint32_t>& indices) = 0;

    /**
     * @brief Create and compile shader program
     *
     * @return true Shader compilation and linking succeeded.
     * @return false Shader compilation or linking failed.
     */
    virtual bool CreateShaders();

    /**
     * @brief Upload geometry to GPU
     */
    void UploadGeometry();

    //=============================================================================================
    // Protected Members
    //=============================================================================================

    QOpenGLFunctions_4_5_Core* m_GL{nullptr};        ///< OpenGL function pointers
    bool                       m_Initialized{false}; ///< Initialization state

    // Mesh parameters
    glm::vec3  m_MeshColor{0.8F, 0.8F, 0.8F};   ///< Mesh color (light gray)
    glm::vec3  m_Position{0.0F, 0.0F, 0.0F};    ///< World position
    RenderMode m_RenderMode{RenderMode::SOLID}; ///< Rendering mode

    // Geometry data
    std::vector<Vertex>   m_Vertices; ///< Vertex data
    std::vector<uint32_t> m_Indices;  ///< Index data

    // OpenGL resources
    std::unique_ptr<QOpenGLShaderProgram>     m_ShaderProgram;
    std::unique_ptr<QOpenGLVertexArrayObject> m_VAO;
    std::unique_ptr<QOpenGLBuffer>            m_VBO;
    std::unique_ptr<QOpenGLBuffer>            m_EBO;
};

} // namespace Orogena::Render
