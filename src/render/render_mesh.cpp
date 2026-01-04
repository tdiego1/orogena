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
    m_ShaderProgram->bind();

    // Create model matrix (translation only for now)
    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0F), m_Position);
    glm::mat4 mvp = projectionMatrix * viewMatrix * model_matrix;

    // Set uniforms
    m_ShaderProgram->setUniformValue("uModelViewProjection",
                                     QMatrix4x4(glm::value_ptr(mvp)).transposed());
    m_ShaderProgram->setUniformValue("uModel",
                                     QMatrix4x4(glm::value_ptr(model_matrix)).transposed());
    m_ShaderProgram->setUniformValue("uView", QMatrix4x4(glm::value_ptr(viewMatrix)).transposed());
    m_ShaderProgram->setUniformValue("uProjection",
                                     QMatrix4x4(glm::value_ptr(projectionMatrix)).transposed());
    m_ShaderProgram->setUniformValue("uMeshColor",
                                     QVector3D(m_MeshColor.x, m_MeshColor.y, m_MeshColor.z));
    m_ShaderProgram->setUniformValue("uLightDirection", QVector3D(0.0F, 1.0F, 1.0F));

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

    m_ShaderProgram->release();
}

//=================================================================================================
// Protected Functions
//=================================================================================================

bool Mesh::CreateShaders()
{
    m_ShaderProgram = std::make_unique<QOpenGLShaderProgram>();

    // Vertex shader with Phong lighting
    const char* vertex_shader_source = R"(
        #version 450 core
        layout(location = 0) in vec3 aPosition;
        layout(location = 1) in vec3 aNormal;
        layout(location = 2) in vec2 aUV;

        uniform mat4 uModelViewProjection;
        uniform mat4 uModel;
        uniform mat4 uView;
        uniform mat4 uProjection;

        out vec3 vNormal;
        out vec3 vFragPos;
        out vec2 vUV;

        void main()
        {
            gl_Position = uModelViewProjection * vec4(aPosition, 1.0);
            vFragPos = vec3(uModel * vec4(aPosition, 1.0));
            vNormal = mat3(transpose(inverse(uModel))) * aNormal;
            vUV = aUV;
        }
    )";

    // Fragment shader with basic Phong lighting
    const char* fragment_shader_source = R"(
        #version 450 core
        in vec3 vNormal;
        in vec3 vFragPos;
        in vec2 vUV;

        uniform vec3 uMeshColor;
        uniform vec3 uLightDirection;

        out vec4 FragColor;

        void main()
        {
            // Normalize inputs
            vec3 normal = normalize(vNormal);
            vec3 lightDir = normalize(uLightDirection);

            // Ambient lighting
            float ambientStrength = 0.3;
            vec3 ambient = ambientStrength * uMeshColor;

            // Diffuse lighting
            float diff = max(dot(normal, lightDir), 0.0);
            vec3 diffuse = diff * uMeshColor;

            // Combine lighting components
            vec3 result = ambient + diffuse;
            FragColor = vec4(result, 1.0);
        }
    )";

    if (!m_ShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex_shader_source))
    {
        Log::Error("Mesh: Failed to compile vertex shader: {}",
                   m_ShaderProgram->log().toStdString());
        return false;
    }

    if (!m_ShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragment_shader_source))
    {
        Log::Error("Mesh: Failed to compile fragment shader: {}",
                   m_ShaderProgram->log().toStdString());
        return false;
    }

    if (!m_ShaderProgram->link())
    {
        Log::Error("Mesh: Failed to link shader program: {}", m_ShaderProgram->log().toStdString());
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

    // Set vertex attribute pointers
    m_ShaderProgram->bind();

    // Position attribute (location = 0)
    m_ShaderProgram->enableAttributeArray(0);
    m_ShaderProgram->setAttributeBuffer(0, GL_FLOAT, offsetof(Vertex, position), 3, sizeof(Vertex));

    // Normal attribute (location = 1)
    m_ShaderProgram->enableAttributeArray(1);
    m_ShaderProgram->setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, normal), 3, sizeof(Vertex));

    // UV attribute (location = 2)
    m_ShaderProgram->enableAttributeArray(2);
    m_ShaderProgram->setAttributeBuffer(2, GL_FLOAT, offsetof(Vertex, uv), 2, sizeof(Vertex));

    m_ShaderProgram->release();

    // Unbind VBO (but keep EBO bound to VAO)
    m_VBO->release();

    // Unbind VAO (EBO stays associated with VAO)
    m_VAO->release();

    Log::Debug("Mesh: Geometry uploaded to GPU");
}

} // namespace Orogena::Render
