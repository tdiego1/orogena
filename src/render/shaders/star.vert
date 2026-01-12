#version 450 core

// Star vertex shader
// Renders point sprites for layered star visualization

layout(location = 0) in vec2 aPosition; // Star center position

uniform mat4 uView;
uniform mat4 uProjection;
uniform float uPointSize;

void main()
{
    gl_Position = uProjection * uView * vec4(aPosition, 0.0, 1.0);
    gl_PointSize = uPointSize;
}
