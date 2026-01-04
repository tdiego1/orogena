#version 450 core

/**************************************************************************************************/
/**
 * @file basic.frag
 * @brief Basic fragment shader with Phong lighting and wireframe support
 *
 * @details
 * Implements simple Phong lighting model with:
 * - Ambient lighting (constant base illumination)
 * - Diffuse lighting (directional lighting based on surface normal)
 * - Uniform color or texture sampling
 * - Wireframe mode support
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres
 */
/**************************************************************************************************/

//=================================================================================================
// Input Variables
//=================================================================================================

in vec3 vFragPos;  ///< Fragment position in world space
in vec3 vNormal;   ///< Fragment normal in world space
in vec2 vTexCoord; ///< Fragment texture coordinates

//=================================================================================================
// Uniforms
//=================================================================================================

uniform vec3      uColor;          ///< Base color (RGB)
uniform vec3      uLightDirection; ///< Directional light direction (world space, normalized)
uniform bool      uUseTexture;     ///< Enable texture sampling (false = use uColor)
uniform sampler2D uTexture;        ///< Texture sampler

//=================================================================================================
// Output
//=================================================================================================

out vec4 FragColor; ///< Final fragment color (RGBA)

//=================================================================================================
// Main Function
//=================================================================================================

void main()
{
    // Normalize interpolated normal
    vec3 normal = normalize(vNormal);

    // Normalize light direction
    vec3 lightDir = normalize(uLightDirection);

    // Ambient lighting (constant base illumination)
    float ambientStrength = 0.3;
    vec3  ambient = ambientStrength * uColor;

    // Diffuse lighting (Lambertian reflectance)
    float diff = max(dot(normal, lightDir), 0.0);
    vec3  diffuse = diff * uColor;

    // Combine lighting components
    vec3 result = ambient + diffuse;

    // Apply texture if enabled
    if (uUseTexture)
    {
        vec4 texColor = texture(uTexture, vTexCoord);
        result *= texColor.rgb;
        FragColor = vec4(result, texColor.a);
    }
    else
    {
        FragColor = vec4(result, 1.0);
    }
}
