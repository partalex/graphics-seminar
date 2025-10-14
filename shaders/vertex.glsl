#version 410 core

layout (location = 0) in vec2 aPos;
uniform vec2 center;
uniform float scale;
uniform float time;

void main() {
    // Apply some subtle wave motion based on time
    vec2 pos = aPos;

    // Wave effect
    pos.x += sin(time * 2.0 + pos.y * 3.0) * 0.02;

    // Apply scaling and center position
    pos = pos * scale + center;

    // Set the final position
    gl_Position = vec4(pos, 0.0, 1.0);
}