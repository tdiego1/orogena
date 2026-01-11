#version 450 core

/**************************************************************************************************/
/**
 * @file galaxy.vert
 * @brief Vertex shader for galaxy particle rendering with point sprites
 *
 * @details
 * Renders stars, dust, and H2 regions as textured point sprites with temperature-based coloring.
 * Implements perspective-correct point size scaling based on camera distance.
 *
 * @author Diego Torres
 * @date 2026
 * @copyright Copyright (C) 2026 Diego Torres
 */
/**************************************************************************************************/

//=================================================================================================
// Input Attributes
//=================================================================================================

layout(location = 0) in vec2 aPosition;     ///< 2D position in parsecs (galaxy plane)
layout(location = 1) in float aTemperature; ///< Star temperature in Kelvin
layout(location = 2) in float aMagnitude;   ///< Brightness magnitude (0-1)

//=================================================================================================
// Uniforms
//=================================================================================================

uniform mat4  uView;       ///< View matrix (world -> camera space)
uniform mat4  uProjection; ///< Projection matrix (camera -> clip space)
uniform float uPointScale; ///< Base point size multiplier
uniform float uFoV;        ///< Field of view for distance-based scaling

//=================================================================================================
// Output Variables
//=================================================================================================

out float vTemperature; ///< Temperature passed to fragment shader
out float vMagnitude;   ///< Magnitude passed to fragment shader

//=================================================================================================
// Constants
//=================================================================================================

const float MIN_POINT_SIZE = 1.0;
const float MAX_POINT_SIZE = 128.0;
const float MIN_TEMPERATURE = 1000.0;  // Kelvin
const float MAX_TEMPERATURE = 40000.0; // Kelvin

//=================================================================================================
// Main Function
//=================================================================================================

void main()
{
    // Transform 2D galaxy position to 3D world space (galaxy lies in XY plane, Z=0)
    vec4 worldPos = vec4(aPosition.x, aPosition.y, 0.0, 1.0);

    // Transform to camera space
    vec4 viewPos = uView * worldPos;

    // Calculate distance from camera for perspective scaling
    float distance = length(viewPos.xyz);

    // Perspective-correct point size: larger FoV or closer distance = larger points
    // This mimics how stars appear larger when zoomed in
    gl_PointSize = uPointScale * (uFoV / max(distance, 1.0));

    // Clamp to reasonable range
    gl_PointSize = clamp(gl_PointSize, MIN_POINT_SIZE, MAX_POINT_SIZE);

    // Transform to clip space
    gl_Position = uProjection * viewPos;

    // Pass interpolated data to fragment shader
    vTemperature = aTemperature;
    vMagnitude = aMagnitude;
}