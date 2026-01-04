/**************************************************************************************************/
/**
 * @file render_shader.cpp
 * @brief Implementation of Shader
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

#include "render_shader.h"

#include <vector>

#include <glm/gtc/type_ptr.hpp>

#include "utils/utils_logger.h"

#include <fstream>
#include <sstream>

namespace Orogena::Render
{

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

Shader::Shader(QOpenGLFunctions_4_5_Core* gl) : m_GL(gl)
{
    if (!m_GL)
    {
        Log::Error("Shader: OpenGL function pointers are null");
    }
}

Shader::~Shader()
{
    Cleanup();
}

Shader::Shader(Shader&& other) noexcept
    : m_GL(other.m_GL),
      m_ProgramID(other.m_ProgramID),
      m_UniformLocationCache(std::move(other.m_UniformLocationCache)),
      m_VertexPath(std::move(other.m_VertexPath)),
      m_FragmentPath(std::move(other.m_FragmentPath))
{
    // Invalidate moved-from object
    other.m_GL = nullptr;
    other.m_ProgramID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other)
    {
        // Clean up existing resources
        Cleanup();

        // Move resources
        m_GL = other.m_GL;
        m_ProgramID = other.m_ProgramID;
        m_UniformLocationCache = std::move(other.m_UniformLocationCache);
        m_VertexPath = std::move(other.m_VertexPath);
        m_FragmentPath = std::move(other.m_FragmentPath);

        // Invalidate moved-from object
        other.m_GL = nullptr;
        other.m_ProgramID = 0;
    }
    return *this;
}

//=================================================================================================
// Shader Loading & Compilation
//=================================================================================================

bool Shader::LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath)
{
    // Store paths for hot-reloading
    m_VertexPath = vertexPath;
    m_FragmentPath = fragmentPath;

    // Read shader source from files
    auto vertexSource = ReadShaderFile(vertexPath);
    if (!vertexSource.has_value())
    {
        Log::Error("Shader: Failed to read vertex shader file: {}", vertexPath);
        return false;
    }

    auto fragmentSource = ReadShaderFile(fragmentPath);
    if (!fragmentSource.has_value())
    {
        Log::Error("Shader: Failed to read fragment shader file: {}", fragmentPath);
        return false;
    }

    // Compile shaders
    return LoadFromSource(vertexSource.value(), fragmentSource.value());
}

bool Shader::LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource)
{
    if (!m_GL)
    {
        Log::Error("Shader: OpenGL function pointers are null");
        return false;
    }

    // Clean up any existing shader program
    Cleanup();

    // Compile vertex shader
    auto vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER, "vertex");
    if (!vertexShader.has_value())
    {
        return false;
    }

    // Compile fragment shader
    auto fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER, "fragment");
    if (!fragmentShader.has_value())
    {
        m_GL->glDeleteShader(vertexShader.value());
        return false;
    }

    // Link shader program
    bool success = LinkProgram(vertexShader.value(), fragmentShader.value());

    // Delete shader objects (no longer needed after linking)
    m_GL->glDeleteShader(vertexShader.value());
    m_GL->glDeleteShader(fragmentShader.value());

    if (success)
    {
        Log::Info("Shader: Program compiled and linked successfully (ID: {})", m_ProgramID);
    }

    return success;
}

bool Shader::Reload()
{
    if (m_VertexPath.empty() || m_FragmentPath.empty())
    {
        Log::Warn("Shader: Cannot reload - shaders were not loaded from files");
        return false;
    }

    Log::Info("Shader: Reloading shaders from files...");
    return LoadFromFiles(m_VertexPath, m_FragmentPath);
}

//=================================================================================================
// Shader Binding
//=================================================================================================

void Shader::Bind() const
{
    if (m_GL && m_ProgramID != 0)
    {
        m_GL->glUseProgram(m_ProgramID);
    }
}

void Shader::Unbind() const
{
    if (m_GL)
    {
        m_GL->glUseProgram(0);
    }
}

//=================================================================================================
// Uniform Setters
//=================================================================================================

void Shader::SetUniform(const std::string& name, int32_t value)
{
    if (!m_GL || m_ProgramID == 0)
    {
        return;
    }

    int32_t location = GetUniformLocation(name);
    if (location != -1)
    {
        m_GL->glUniform1i(location, value);
    }
}

void Shader::SetUniform(const std::string& name, float32_t value)
{
    if (!m_GL || m_ProgramID == 0)
    {
        return;
    }

    int32_t location = GetUniformLocation(name);
    if (location != -1)
    {
        m_GL->glUniform1f(location, value);
    }
}

void Shader::SetUniform(const std::string& name, bool value)
{
    SetUniform(name, static_cast<int32_t>(value));
}

void Shader::SetUniform(const std::string& name, const glm::vec2& value)
{
    if (!m_GL || m_ProgramID == 0)
    {
        return;
    }

    int32_t location = GetUniformLocation(name);
    if (location != -1)
    {
        m_GL->glUniform2fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::SetUniform(const std::string& name, const glm::vec3& value)
{
    if (!m_GL || m_ProgramID == 0)
    {
        return;
    }

    int32_t location = GetUniformLocation(name);
    if (location != -1)
    {
        m_GL->glUniform3fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::SetUniform(const std::string& name, const glm::vec4& value)
{
    if (!m_GL || m_ProgramID == 0)
    {
        return;
    }

    int32_t location = GetUniformLocation(name);
    if (location != -1)
    {
        m_GL->glUniform4fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::SetUniform(const std::string& name, const glm::mat3& value)
{
    if (!m_GL || m_ProgramID == 0)
    {
        return;
    }

    int32_t location = GetUniformLocation(name);
    if (location != -1)
    {
        m_GL->glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void Shader::SetUniform(const std::string& name, const glm::mat4& value)
{
    if (!m_GL || m_ProgramID == 0)
    {
        return;
    }

    int32_t location = GetUniformLocation(name);
    if (location != -1)
    {
        m_GL->glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

//=================================================================================================
// Private Helper Functions
//=================================================================================================

std::optional<uint32_t>
Shader::CompileShader(const std::string& source, uint32_t type, const std::string& shaderName)
{
    // Create shader object
    uint32_t shader = m_GL->glCreateShader(type);
    if (shader == 0)
    {
        Log::Error("Shader: Failed to create {} shader object", shaderName);
        return std::nullopt;
    }

    // Compile shader
    const char* sourceCStr = source.c_str();
    m_GL->glShaderSource(shader, 1, &sourceCStr, nullptr);
    m_GL->glCompileShader(shader);

    // Check compilation status
    int32_t success = 0;
    m_GL->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == 0)
    {
        // Get error message length
        int32_t logLength = 0;
        m_GL->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        // Retrieve error message
        std::vector<char> errorLog(static_cast<size_t>(logLength));
        m_GL->glGetShaderInfoLog(shader, logLength, nullptr, errorLog.data());

        Log::Error("Shader: {} shader compilation failed:\n{}", shaderName, errorLog.data());

        m_GL->glDeleteShader(shader);
        return std::nullopt;
    }

    Log::Debug("Shader: {} shader compiled successfully", shaderName);
    return shader;
}

bool Shader::LinkProgram(uint32_t vertexShader, uint32_t fragmentShader)
{
    // Create program object
    m_ProgramID = m_GL->glCreateProgram();
    if (m_ProgramID == 0)
    {
        Log::Error("Shader: Failed to create shader program");
        return false;
    }

    // Attach shaders
    m_GL->glAttachShader(m_ProgramID, vertexShader);
    m_GL->glAttachShader(m_ProgramID, fragmentShader);

    // Link program
    m_GL->glLinkProgram(m_ProgramID);

    // Check linking status
    int32_t success = 0;
    m_GL->glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);

    if (success == 0)
    {
        // Get error message length
        int32_t logLength = 0;
        m_GL->glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &logLength);

        // Retrieve error message
        std::vector<char> errorLog(static_cast<size_t>(logLength));
        m_GL->glGetProgramInfoLog(m_ProgramID, logLength, nullptr, errorLog.data());

        Log::Error("Shader: Program linking failed:\n{}", errorLog.data());

        m_GL->glDeleteProgram(m_ProgramID);
        m_ProgramID = 0;
        return false;
    }

    // Detach shaders after successful linking
    m_GL->glDetachShader(m_ProgramID, vertexShader);
    m_GL->glDetachShader(m_ProgramID, fragmentShader);

    Log::Debug("Shader: Program linked successfully");
    return true;
}

std::optional<std::string> Shader::ReadShaderFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        Log::Error("Shader: Failed to open shader file: {}", path);
        return std::nullopt;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::string source = buffer.str();
    if (source.empty())
    {
        Log::Warn("Shader: Shader file is empty: {}", path);
    }

    return source;
}

int32_t Shader::GetUniformLocation(const std::string& name)
{
    // Check cache first
    auto it = m_UniformLocationCache.find(name);
    if (it != m_UniformLocationCache.end())
    {
        return it->second;
    }

    // Query OpenGL for uniform location
    int32_t location = m_GL->glGetUniformLocation(m_ProgramID, name.c_str());

    // Cache the location (even if -1, to avoid repeated failed queries)
    m_UniformLocationCache[name] = location;

    if (location == -1)
    {
        Log::Warn("Shader: Uniform '{}' not found in shader program (ID: {})", name, m_ProgramID);
    }

    return location;
}

void Shader::Cleanup()
{
    if (m_GL && m_ProgramID != 0)
    {
        m_GL->glDeleteProgram(m_ProgramID);
        m_ProgramID = 0;
    }

    m_UniformLocationCache.clear();
}

} // namespace Orogena::Render
