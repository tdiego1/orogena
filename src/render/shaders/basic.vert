#version 450 core

/**************************************************************************************************/
/**
 * @file basic.vert
 * @brief Basic vertex shader with MVP transformation and Phong lighting preparation
 *
 * @details
 * Transforms vertices by Model-View-Projection matrix and prepares data for Phong lighting.
 * Passes normals and UVs to fragment shader.
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres
 */
/**************************************************************************************************/

//=================================================================================================
// Input Attributes
//=================================================================================================

layout(location = 0) in vec3 aPosition; ///< Vertex position in model space
layout(location = 1) in vec3 aNormal;   ///< Vertex normal in model space
layout(location = 2) in vec2 aTexCoord; ///< Texture coordinates

//=================================================================================================
// Uniforms
//=================================================================================================

uniform mat4 uModel;      ///< Model matrix (model -> world space)
uniform mat4 uView;       ///< View matrix (world -> view space)
uniform mat4 uProjection; ///< Projection matrix (view -> clip space)

//=================================================================================================
// Output Variables
//=================================================================================================

out vec3 vFragPos;  ///< Fragment position in world space
out vec3 vNormal;   ///< Fragment normal in world space
out vec2 vTexCoord; ///< Fragment texture coordinates

//=================================================================================================
// Main Function
//=================================================================================================

void main()
{
    // Transform position to world space
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    vFragPos = worldPos.xyz;

    // Transform normal to world space (using normal matrix to handle non-uniform scaling)
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;

    // Pass texture coordinates to fragment shader
    vTexCoord = aTexCoord;

    // Transform position to clip space
    gl_Position = uProjection * uView * worldPos;
}
