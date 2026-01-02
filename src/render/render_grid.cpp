/**************************************************************************************************/
/**
 * @file render_grid.cpp
 * @brief Implementation of Grid
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

#include "render_grid.h"

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils/utils_logger.h"

namespace Orogena::Render
{

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

Grid::Grid(QOpenGLFunctions_4_5_Core* gl) : m_GL(gl)
{
    Log::Debug("Grid: Created");
}

Grid::~Grid()
{
    // Qt manages OpenGL resource cleanup
    Log::Debug("Grid: Destroyed");
}

//=================================================================================================
// Public Functions
//=================================================================================================

bool Grid::Initialize()
{
    if (m_Initialized)
    {
        Log::Warn("Grid: Already initialized");
        return true;
    }

    // Create shader program
    m_ShaderProgram = std::make_unique<QOpenGLShaderProgram>();

    // Vertex shader - simple pass-through with MVP transform
    const char* vertex_shader_source = R"(
        #version 450 core
        layout(location = 0) in vec3 aPosition;

        uniform mat4 uModelViewProjection;

        void main()
        {
            gl_Position = uModelViewProjection * vec4(aPosition, 1.0);
        }
    )";

    // Fragment shader - solid color
    const char* fragment_shader_source = R"(
        #version 450 core
        out vec4 FragColor;

        uniform vec3 uColor;

        void main()
        {
            FragColor = vec4(uColor, 1.0);
        }
    )";

    if (!m_ShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex_shader_source))
    {
        Log::Error("Grid: Failed to compile vertex shader: {}",
                   m_ShaderProgram->log().toStdString());
        return false;
    }

    if (!m_ShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragment_shader_source))
    {
        Log::Error("Grid: Failed to compile fragment shader: {}",
                   m_ShaderProgram->log().toStdString());
        return false;
    }

    if (!m_ShaderProgram->link())
    {
        Log::Error("Grid: Failed to link shader program: {}", m_ShaderProgram->log().toStdString());
        return false;
    }

    Log::Info("Grid: Shader program compiled and linked successfully");

    // Create VAO and VBO
    m_VAO = std::make_unique<QOpenGLVertexArrayObject>();
    if (!m_VAO->create())
    {
        Log::Error("Grid: Failed to create VAO");
        return false;
    }

    m_VBO = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    if (!m_VBO->create())
    {
        Log::Error("Grid: Failed to create VBO");
        return false;
    }

    // Generate grid geometry
    GenerateGridGeometry();

    m_Initialized = true;
    Log::Info("Grid: Initialized successfully (size={}, spacing={})", m_GridSize, m_GridSpacing);

    return true;
}

void Grid::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    if (!m_Initialized || m_VertexCount == 0)
    {
        return;
    }

    // Bind shader program
    m_ShaderProgram->bind();

    // Set uniforms
    glm::mat4 mvp = projectionMatrix * viewMatrix;
    m_ShaderProgram->setUniformValue("uModelViewProjection",
                                     QMatrix4x4(glm::value_ptr(mvp)).transposed());
    m_ShaderProgram->setUniformValue("uColor",
                                     QVector3D(m_GridColor.x, m_GridColor.y, m_GridColor.z));

    // Bind VAO and draw
    m_VAO->bind();
    m_GL->glDrawArrays(GL_LINES, 0, m_VertexCount);
    m_VAO->release();

    m_ShaderProgram->release();
}

void Grid::SetGridParameters(float32_t size, float32_t spacing)
{
    m_GridSize = size;
    m_GridSpacing = spacing;

    if (m_Initialized)
    {
        GenerateGridGeometry();
        Log::Debug("Grid: Parameters updated (size={}, spacing={})", size, spacing);
    }
}

//=================================================================================================
// Private Functions
//=================================================================================================

void Grid::GenerateGridGeometry()
{
    std::vector<glm::vec3> vertices;

    // Calculate number of lines in each direction
    int32_t   num_lines = static_cast<int32_t>(m_GridSize / m_GridSpacing) + 1;
    float32_t half_size = m_GridSize / 2.0F;

    // Generate lines parallel to X axis
    for (int32_t i = 0; i < num_lines; ++i)
    {
        float32_t z = -half_size + static_cast<float32_t>(i) * m_GridSpacing;
        vertices.emplace_back(-half_size, 0.0F, z);
        vertices.emplace_back(half_size, 0.0F, z);
    }

    // Generate lines parallel to Z axis
    for (int32_t i = 0; i < num_lines; ++i)
    {
        float32_t x = -half_size + static_cast<float32_t>(i) * m_GridSpacing;
        vertices.emplace_back(x, 0.0F, -half_size);
        vertices.emplace_back(x, 0.0F, half_size);
    }

    m_VertexCount = static_cast<int32_t>(vertices.size());

    // Upload vertex data to GPU
    m_VAO->bind();
    m_VBO->bind();
    m_VBO->allocate(vertices.data(), m_VertexCount * static_cast<int32_t>(sizeof(glm::vec3)));

    // Set vertex attribute pointers
    m_ShaderProgram->bind();
    m_ShaderProgram->enableAttributeArray(0);
    m_ShaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(glm::vec3));
    m_ShaderProgram->release();

    m_VBO->release();
    m_VAO->release();

    Log::Debug("Grid: Generated {} vertices ({} lines)", m_VertexCount, m_VertexCount / 2);
}

} // namespace Orogena::Render
