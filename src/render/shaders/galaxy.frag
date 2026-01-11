#version 450 core

/**************************************************************************************************/
/**
 * @file galaxy.frag
 * @brief Fragment shader for galaxy particle rendering with spectral coloring
 *
 * @details
 * Applies temperature-based coloring using a 1D color ramp texture derived from blackbody
 * radiation. Uses a soft point sprite texture for realistic star/dust appearance with
 * additive blending.
 *
 * @author Diego Torres
 * @date 2026
 * @copyright Copyright (C) 2026 Diego Torres
 */
/**************************************************************************************************/

//=================================================================================================
// Input Variables
//=================================================================================================

in float vTemperature; ///< Star temperature in Kelvin
in float vMagnitude;   ///< Brightness magnitude (0-1)

//=================================================================================================
// Uniforms
//=================================================================================================

uniform sampler2D uPointTexture; ///< Point sprite texture (soft glow, white with alpha)
uniform sampler1D uColorRamp;    ///< 1D texture mapping temperature to RGB color

//=================================================================================================
// Output
//=================================================================================================

out vec4 FragColor; ///< Final fragment color (RGBA)

//=================================================================================================
// Constants
//=================================================================================================

const float MIN_TEMPERATURE = 1000.0;  // Kelvin (coolest red dwarf)
const float MAX_TEMPERATURE = 40000.0; // Kelvin (hottest O-type star)

//=================================================================================================
// Main Function
//=================================================================================================

void main()
{
    // Sample point sprite texture for soft circular falloff
    // gl_PointCoord provides UV coordinates [0, 1] for the point sprite
    vec4 texColor = texture(uPointTexture, gl_PointCoord);

    // Normalize temperature to [0, 1] range for texture lookup
    float tempNorm =
        clamp((vTemperature - MIN_TEMPERATURE) / (MAX_TEMPERATURE - MIN_TEMPERATURE), 0.0, 1.0);

    // Sample color ramp: red (cool) -> yellow -> white -> blue (hot)
    vec3 starColor = texture(uColorRamp, tempNorm).rgb;

    // Combine star color with magnitude and sprite alpha
    // Magnitude controls brightness, sprite alpha provides soft edges
    vec3  finalColor = starColor * vMagnitude;
    float finalAlpha = texColor.a * vMagnitude;

    // Output final color with alpha for blending
    FragColor = vec4(finalColor, finalAlpha);
}