/**************************************************************************************************/
/**
 * @file test_shader.cpp
 * @brief Unit tests for Shader class
 *
 * @details
 * Tests shader compilation, linking, uniform management, and error handling.
 * Note: These tests verify API behavior and error handling. Full OpenGL rendering
 * tests require an active OpenGL context and are better suited for integration tests.
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

#include "render/render_shader.h"

#include <gtest/gtest.h>

namespace Orogena::Render
{

//=================================================================================================
// Test Fixture
//=================================================================================================

/**
 * @brief Test fixture for Shader class
 *
 * @details
 * Note: Most Shader tests require an active OpenGL context. These basic tests verify
 * API behavior and error handling without requiring a full OpenGL setup.
 * For full shader compilation and rendering tests, use integration tests with OpenGL context.
 */
class ShaderTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Note: Cannot create actual shader without OpenGL context
        // Tests verify API behavior and error handling
    }

    void TearDown() override
    {
        // Cleanup
    }
};

//=================================================================================================
// Constructor Tests
//=================================================================================================

TEST_F(ShaderTest, ConstructorAcceptsNullGLPointer)
{
    // Should handle null GL pointer gracefully
    Shader shader(nullptr);

    // Should not be valid without GL functions
    EXPECT_FALSE(shader.IsValid());
    EXPECT_EQ(shader.GetProgramID(), 0U);
}

//=================================================================================================
// File Path Tests
//=================================================================================================

TEST_F(ShaderTest, LoadFromFilesHandlesInvalidPaths)
{
    // Create shader with null GL (will fail to load, but API should handle gracefully)
    Shader shader(nullptr);

    // Try to load from non-existent files
    bool success = shader.LoadFromFiles("nonexistent.vert", "nonexistent.frag");

    // Should fail gracefully
    EXPECT_FALSE(success);
    EXPECT_FALSE(shader.IsValid());
}

TEST_F(ShaderTest, ReloadWithoutPreviousFilesReturnsError)
{
    Shader shader(nullptr);

    // Try to reload without loading from files first
    bool success = shader.Reload();

    // Should fail gracefully
    EXPECT_FALSE(success);
}

//=================================================================================================
// Move Semantics Tests
//=================================================================================================

TEST_F(ShaderTest, MoveConstructorTransfersOwnership)
{
    Shader shader1(nullptr);

    // Move construct
    Shader shader2(std::move(shader1));

    // shader1 should be invalidated
    EXPECT_EQ(shader1.GetProgramID(), 0U); // NOLINT

    // shader2 should have the resources (though still 0 without actual GL)
    EXPECT_EQ(shader2.GetProgramID(), 0U);
}

TEST_F(ShaderTest, MoveAssignmentTransfersOwnership)
{
    Shader shader1(nullptr);
    Shader shader2(nullptr);

    // Move assign
    shader2 = std::move(shader1);

    // shader1 should be invalidated
    EXPECT_EQ(shader1.GetProgramID(), 0U); // NOLINT

    // shader2 should have the resources
    EXPECT_EQ(shader2.GetProgramID(), 0U);
}

//=================================================================================================
// Shader Source Validation Tests
//=================================================================================================

TEST_F(ShaderTest, LoadFromSourceHandlesEmptyShaders)
{
    Shader shader(nullptr);

    // Try to load empty shader source
    bool success = shader.LoadFromSource("", "");

    // Should fail gracefully
    EXPECT_FALSE(success);
    EXPECT_FALSE(shader.IsValid());
}

TEST_F(ShaderTest, LoadFromSourceHandlesInvalidGLSL)
{
    Shader shader(nullptr);

    // Invalid GLSL (missing version directive, syntax errors)
    const std::string invalid_vertex = "invalid glsl code";
    const std::string invalid_fragment = "more invalid code";

    bool success = shader.LoadFromSource(invalid_vertex, invalid_fragment);

    // Should fail (no GL context to compile, but API should handle gracefully)
    EXPECT_FALSE(success);
}

//=================================================================================================
// API Behavior Tests
//=================================================================================================

TEST_F(ShaderTest, BindUnbindDoNotCrashWithoutContext)
{
    Shader shader(nullptr);

    // These should not crash even without valid GL context or program
    EXPECT_NO_THROW(shader.Bind());
    EXPECT_NO_THROW(shader.Unbind());
}

TEST_F(ShaderTest, UniformSettersDoNotCrashWithInvalidShader)
{
    Shader shader(nullptr);

    // Uniform setters should handle invalid shader gracefully
    EXPECT_NO_THROW(shader.SetUniform("uTest", 42));
    EXPECT_NO_THROW(shader.SetUniform("uTest", 3.14F));
    EXPECT_NO_THROW(shader.SetUniform("uTest", true));
    EXPECT_NO_THROW(shader.SetUniform("uTest", glm::vec2(1.0F, 2.0F)));
    EXPECT_NO_THROW(shader.SetUniform("uTest", glm::vec3(1.0F, 2.0F, 3.0F)));
    EXPECT_NO_THROW(shader.SetUniform("uTest", glm::vec4(1.0F, 2.0F, 3.0F, 4.0F)));
    EXPECT_NO_THROW(shader.SetUniform("uTest", glm::mat3(1.0F)));
    EXPECT_NO_THROW(shader.SetUniform("uTest", glm::mat4(1.0F)));
}

//=================================================================================================
// State Management Tests
//=================================================================================================

TEST_F(ShaderTest, IsValidReturnsFalseForNewShader)
{
    Shader shader(nullptr);

    // New shader without successful compilation should not be valid
    EXPECT_FALSE(shader.IsValid());
}

TEST_F(ShaderTest, GetProgramIDReturnsZeroForInvalidShader)
{
    Shader shader(nullptr);

    // Invalid shader should have program ID of 0
    EXPECT_EQ(shader.GetProgramID(), 0U);
}

//=================================================================================================
// Integration Note
//=================================================================================================

/**
 * @note Integration Tests Required
 *
 * The following functionality requires an active OpenGL context and should be tested
 * in integration tests:
 *
 * 1. Actual shader compilation from valid GLSL source
 * 2. Shader linking success/failure
 * 3. Uniform location querying and caching
 * 4. Shader hot-reloading from files
 * 5. Error messages with line numbers from OpenGL
 * 6. Rendering with compiled shaders
 *
 * These integration tests should:
 * - Create a QOffscreenSurface with OpenGL context
 * - Load the basic.vert and basic.frag shaders
 * - Verify successful compilation and linking
 * - Test uniform setters with valid shader program
 * - Test shader reloading functionality
 */

} // namespace Orogena::Render
