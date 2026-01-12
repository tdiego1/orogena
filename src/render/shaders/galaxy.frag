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

uniform sampler2D uPointTexture;  ///< Point sprite texture (soft glow, white with alpha)
uniform sampler1D uColorRamp;     ///< 1D texture mapping temperature to RGB color
uniform float     uBrightBoost;   ///< Brightness boost for bright stars (0.0 for normal, 0.2 for bright)
uniform vec3      uColorMultiplier; ///< RGB color multiplier (e.g., vec3(2,0.5,0.5) for H2 red-shift)

//=================================================================================================
// Output
//=================================================================================================

out vec4 FragColor; ///< Final fragment color (RGBA)

//=================================================================================================
// Constants
//=================================================================================================

const float MIN_TEMPERATURE = 1000.0;  // Kelvin (coolest red dwarf)
const float MAX_TEMPERATURE = 10000.0; // Kelvin - matches original Galaxy-Renderer range

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

    // Apply color multiplier (for H2 regions: red=2x, green/blue=0.5x)
    starColor *= uColorMultiplier;

    // Combine star color with magnitude and sprite alpha
    // Original formula: color = (spectralColor * magnitude) + boost
    // Alpha comes from texture only - magnitude only affects RGB brightness
    // For H2 white centers: uBrightBoost = 1.0 overrides to pure white
    vec3 finalColor;
    if (uBrightBoost >= 1.0)
    {
        // Pure white override for H2 region centers
        finalColor = vec3(1.0, 1.0, 1.0);
    }
    else
    {
        // Normal rendering: color * magnitude + brightness boost
        finalColor = starColor * vMagnitude + vec3(uBrightBoost);
    }

    // Use texture alpha directly - don't multiply by magnitude
    // This matches original Galaxy-Renderer behavior
    float finalAlpha = texColor.a;

    // Output final color with alpha for blending
    FragColor = vec4(finalColor, finalAlpha);
}