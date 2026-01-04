/**************************************************************************************************/
/**
 * @file render_shader.h
 * @brief OpenGL shader program management with RAII and uniform caching
 *
 * @details
 * Provides a robust shader compilation and management system for OpenGL 4.5+:
 * - Compile and link vertex/fragment shaders from files or source code
 * - Detailed error handling with line numbers
 * - Type-safe uniform setters with location caching
 * - RAII resource management
 * - Shader hot-reloading support (debug builds)
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

#include <string>

#include <QOpenGLFunctions_4_5_Core>

#include <glm/glm.hpp>

#include "utils/utils_types.h"

#include <optional>
#include <unordered_map>

namespace Orogena::Render
{

/**************************************************************************************************/
/**
 * @brief OpenGL shader program with compilation, linking, and uniform management
 *
 * @details
 * Manages the lifecycle of OpenGL shader programs with the following features:
 *
 * **Shader Loading**:
 * - Load from files (.vert, .frag, .glsl)
 * - Compile from source strings
 * - Automatic shader cleanup on destruction
 *
 * **Error Handling**:
 * - Detailed compilation errors with line numbers
 * - Linking error messages
 * - Validation before use
 *
 * **Uniform Management**:
 * - Type-safe uniform setters for scalars, vectors, matrices
 * - Automatic uniform location caching for performance
 * - Sampler support (sampler2D)
 *
 * **RAII & Safety**:
 * - Automatic shader cleanup on destruction
 * - Move semantics (no copying)
 * - Validation of shader program before use
 *
 * **Example Usage**:
 * @code
 * auto shader = std::make_unique<Shader>(gl);
 * if (shader->LoadFromFiles("shaders/basic.vert", "shaders/basic.frag"))
 * {
 *     shader->Bind();
 *     shader->SetUniform("uMVP", mvpMatrix);
 *     shader->SetUniform("uColor", glm::vec3(1.0f, 0.0f, 0.0f));
 *     // Draw calls...
 *     shader->Unbind();
 * }
 * @endcode
 */
class Shader
{
  public:
    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    /**
     * @brief Construct a new Shader manager
     *
     * @param gl OpenGL function pointers (must remain valid for shader lifetime).
     */
    explicit Shader(QOpenGLFunctions_4_5_Core* gl);

    /**
     * @brief Destroy the Shader and clean up OpenGL resources
     *
     * @details Automatically deletes shader program and individual shader objects.
     */
    ~Shader();

    // Delete copy operations - shaders own GPU resources
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Allow move operations
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    //=============================================================================================
    // Shader Loading & Compilation
    //=============================================================================================

    /**
     * @brief Load and compile shaders from file paths
     *
     * @param vertexPath Path to vertex shader file (.vert).
     * @param fragmentPath Path to fragment shader file (.frag).
     * @return true if compilation and linking succeeded, false otherwise.
     *
     * @details
     * Reads shader source from files, compiles them, and links the program.
     * Error messages are logged with file names and line numbers.
     */
    bool LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);

    /**
     * @brief Compile shaders from source code strings
     *
     * @param vertexSource Vertex shader GLSL source code.
     * @param fragmentSource Fragment shader GLSL source code.
     * @return true if compilation and linking succeeded, false otherwise.
     *
     * @details
     * Compiles shaders from provided source strings and links the program.
     * Useful for embedded shaders or runtime-generated code.
     */
    bool LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource);

    /**
     * @brief Reload shaders from previously loaded files
     *
     * @return true if reload succeeded, false otherwise.
     *
     * @details
     * Hot-reloads shaders from the last used file paths.
     * Only available if shaders were loaded via LoadFromFiles().
     * Useful for shader development and debugging.
     */
    bool Reload();

    //=============================================================================================
    // Shader Binding
    //=============================================================================================

    /**
     * @brief Bind this shader program for rendering
     *
     * @details Makes this shader program active for subsequent draw calls.
     */
    void Bind() const;

    /**
     * @brief Unbind the currently active shader program
     *
     * @details Sets OpenGL shader program to 0 (no program).
     */
    void Unbind() const;

    /**
     * @brief Check if shader program is valid and ready to use
     *
     * @return true if program is compiled and linked, false otherwise.
     */
    [[nodiscard]] bool IsValid() const
    {
        return m_ProgramID != 0;
    }

    /**
     * @brief Get OpenGL program ID
     *
     * @return uint32_t OpenGL program handle (0 if invalid).
     */
    [[nodiscard]] uint32_t GetProgramID() const
    {
        return m_ProgramID;
    }

    //=============================================================================================
    // Uniform Setters (with automatic location caching)
    //=============================================================================================

    /**
     * @brief Set int uniform
     *
     * @param name Uniform variable name.
     * @param value Integer value.
     */
    void SetUniform(const std::string& name, int32_t value);

    /**
     * @brief Set float uniform
     *
     * @param name Uniform variable name.
     * @param value Float value.
     */
    void SetUniform(const std::string& name, float32_t value);

    /**
     * @brief Set bool uniform
     *
     * @param name Uniform variable name.
     * @param value Boolean value (converted to int).
     */
    void SetUniform(const std::string& name, bool value);

    /**
     * @brief Set vec2 uniform
     *
     * @param name Uniform variable name.
     * @param value 2D vector.
     */
    void SetUniform(const std::string& name, const glm::vec2& value);

    /**
     * @brief Set vec3 uniform
     *
     * @param name Uniform variable name.
     * @param value 3D vector.
     */
    void SetUniform(const std::string& name, const glm::vec3& value);

    /**
     * @brief Set vec4 uniform
     *
     * @param name Uniform variable name.
     * @param value 4D vector.
     */
    void SetUniform(const std::string& name, const glm::vec4& value);

    /**
     * @brief Set mat3 uniform
     *
     * @param name Uniform variable name.
     * @param value 3x3 matrix.
     */
    void SetUniform(const std::string& name, const glm::mat3& value);

    /**
     * @brief Set mat4 uniform
     *
     * @param name Uniform variable name.
     * @param value 4x4 matrix.
     */
    void SetUniform(const std::string& name, const glm::mat4& value);

  private:
    //=============================================================================================
    // Private Helper Functions
    //=============================================================================================

    /**
     * @brief Compile a single shader from source
     *
     * @param source Shader source code.
     * @param type Shader type (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER).
     * @param shaderName Shader name for error messages (e.g., "vertex" or "fragment").
     * @return std::optional<uint32_t> Shader ID if successful, std::nullopt otherwise.
     */
    std::optional<uint32_t>
    CompileShader(const std::string& source, uint32_t type, const std::string& shaderName);

    /**
     * @brief Link vertex and fragment shaders into a program
     *
     * @param vertexShader Compiled vertex shader ID.
     * @param fragmentShader Compiled fragment shader ID.
     * @return true if linking succeeded, false otherwise.
     */
    bool LinkProgram(uint32_t vertexShader, uint32_t fragmentShader);

    /**
     * @brief Read shader source code from file
     *
     * @param path File path to shader.
     * @return std::optional<std::string> Shader source if successful, std::nullopt otherwise.
     */
    std::optional<std::string> ReadShaderFile(const std::string& path);

    /**
     * @brief Get uniform location with caching
     *
     * @param name Uniform variable name.
     * @return int32_t Uniform location (-1 if not found).
     *
     * @details
     * Caches uniform locations to avoid repeated glGetUniformLocation calls.
     * Returns -1 if uniform doesn't exist (logs warning on first query).
     */
    int32_t GetUniformLocation(const std::string& name);

    /**
     * @brief Clean up OpenGL shader resources
     *
     * @details Deletes shader program and clears cached data.
     */
    void Cleanup();

    //=============================================================================================
    // Private Members
    //=============================================================================================

    QOpenGLFunctions_4_5_Core* m_GL{nullptr}; ///< OpenGL function pointers

    uint32_t m_ProgramID{0}; ///< OpenGL shader program ID (0 = invalid)

    std::unordered_map<std::string, int32_t> m_UniformLocationCache; ///< Cached uniform locations

    std::string m_VertexPath;   ///< Path to vertex shader file (for reloading)
    std::string m_FragmentPath; ///< Path to fragment shader file (for reloading)
};

} // namespace Orogena::Render
