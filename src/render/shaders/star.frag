#version 450 core

// Star fragment shader
// Applies color, texture, and animations to star point sprites

out vec4 FragColor;

uniform sampler2D uTexture;      // Radial gradient texture
uniform vec4 uColor;             // Star color (from temperature)
uniform float uOpacity;          // Layer opacity
uniform float uTime;             // Animation time
uniform float uRotationAngle;    // Rotation for surface patterns

void main()
{
    // Get texture coordinates (point sprite coords)
    vec2 texCoord = gl_PointCoord;

    // Apply rotation to texture coordinates for surface pattern animation
    // Only rotates photosphere layer (when uRotationAngle != 0)
    if (uRotationAngle > 0.001)
    {
        // Center coordinates
        vec2 centered = texCoord - vec2(0.5, 0.5);

        // Rotate
        float cosAngle = cos(uRotationAngle);
        float sinAngle = sin(uRotationAngle);
        vec2 rotated = vec2(
            centered.x * cosAngle - centered.y * sinAngle,
            centered.x * sinAngle + centered.y * cosAngle
        );

        // Un-center
        texCoord = rotated + vec2(0.5, 0.5);
    }

    // Sample radial gradient texture
    vec4 texColor = texture(uTexture, texCoord);

    // Apply star color and opacity
    FragColor = vec4(uColor.rgb, texColor.a * uOpacity);

    // Discard fully transparent pixels to avoid overdraw
    if (FragColor.a < 0.01)
    {
        discard;
    }
}
