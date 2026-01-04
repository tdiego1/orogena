/**************************************************************************************************/
/**
 * @file render_mesh.cpp
 * @brief Implementation of Mesh
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

#include "render_mesh.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils/utils_logger.h"

namespace Orogena::Render
{

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

Mesh::Mesh(QOpenGLFunctions_4_5_Core* gl) : m_GL(gl)
{
    Log::Debug("Mesh: Created");
}

Mesh::~Mesh()
{
    // Qt manages OpenGL resource cleanup
    Log::Debug("Mesh: Destroyed");
}

//=================================================================================================
// Public Functions
//=================================================================================================

bool Mesh::Initialize()
{
    if (m_Initialized)
    {
        Log::Warn("Mesh: Already initialized");
        return true;
    }

    // Create shader program
    if (!CreateShaders())
    {
        return false;
    }

    // Create VAO and buffers
    m_VAO = std::make_unique<QOpenGLVertexArrayObject>();
    if (!m_VAO->create())
    {
        Log::Error("Mesh: Failed to create VAO");
        return false;
    }

    m_VBO = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    if (!m_VBO->create())
    {
        Log::Error("Mesh: Failed to create VBO");
        return false;
    }

    m_EBO = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    if (!m_EBO->create())
    {
        Log::Error("Mesh: Failed to create EBO");
        return false;
    }

    // Generate geometry
    GenerateGeometry(m_Vertices, m_Indices);

    if (m_Vertices.empty() || m_Indices.empty())
    {
        Log::Error("Mesh: Geometry generation produced no data");
        return false;
    }

    // Upload geometry to GPU
    UploadGeometry();

    m_Initialized = true;
    Log::Info("Mesh: Initialized successfully ({} vertices, {} indices)", m_Vertices.size(),
              m_Indices.size());

    return true;
}

void Mesh::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    if (!m_Initialized || m_Indices.empty())
    {
        return;
    }

    // Bind shader program
    m_Shader->Bind();

    // Create model matrix (translation only for now)
    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0F), m_Position);

    // Set uniforms using our custom Shader class
    m_Shader->SetUniform("uModel", model_matrix);
    m_Shader->SetUniform("uView", viewMatrix);
    m_Shader->SetUniform("uProjection", projectionMatrix);
    m_Shader->SetUniform("uColor", m_MeshColor);
    m_Shader->SetUniform("uLightDirection", glm::vec3(0.0F, 1.0F, 1.0F));
    m_Shader->SetUniform("uUseTexture", false);

    // Set polygon mode based on render mode
    if (m_RenderMode == RenderMode::WIREFRAME)
    {
        m_GL->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        m_GL->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Bind VAO and draw
    m_VAO->bind();
    m_GL->glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, 0);
    m_VAO->release();

    // Reset polygon mode
    m_GL->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    m_Shader->Unbind();
}

//=================================================================================================
// Protected Functions
//=================================================================================================

bool Mesh::CreateShaders()
{
    // Create shader using our custom Shader class
    m_Shader = std::make_unique<Shader>(m_GL);

    // Load shaders from files
    if (!m_Shader->LoadFromFiles("shaders/basic.vert", "shaders/basic.frag"))
    {
        Log::Error("Mesh: Failed to load shader files");
        return false;
    }

    Log::Info("Mesh: Shader program compiled and linked successfully");
    return true;
}

void Mesh::UploadGeometry()
{
    // Bind VAO
    m_VAO->bind();

    // Upload vertex data
    m_VBO->bind();
    m_VBO->allocate(m_Vertices.data(), static_cast<int32_t>(m_Vertices.size() * sizeof(Vertex)));

    // Upload index data
    m_EBO->bind();
    m_EBO->allocate(m_Indices.data(), static_cast<int32_t>(m_Indices.size() * sizeof(uint32_t)));

    // Set vertex attribute pointers using raw OpenGL
    // Position attribute (location = 0)
    m_GL->glEnableVertexAttribArray(0);
    m_GL->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                reinterpret_cast<void*>(offsetof(Vertex, position)));

    // Normal attribute (location = 1)
    m_GL->glEnableVertexAttribArray(1);
    m_GL->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                reinterpret_cast<void*>(offsetof(Vertex, normal)));

    // UV attribute (location = 2)
    m_GL->glEnableVertexAttribArray(2);
    m_GL->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                reinterpret_cast<void*>(offsetof(Vertex, uv)));

    // Unbind VBO (but keep EBO bound to VAO)
    m_VBO->release();

    // Unbind VAO (EBO stays associated with VAO)
    m_VAO->release();

    Log::Debug("Mesh: Geometry uploaded to GPU");
}

} // namespace Orogena::Render
