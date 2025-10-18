#version 410 core

// Uniform variable for time
uniform float time;
// Flag to indicate if the current fragment is part of the border
uniform int isBorder;

out vec4 FragColor;

// Pseudo-random noise function
float random(vec2 st) {
    const float RAND_CONST1 = 12.9898;
    const float RAND_CONST2 = 78.233;
    const float RAND_CONST3 = 43758.5453123;
    // random generator simulated with sine and dot product
    return fract(sin(dot(st.xy, vec2(RAND_CONST1, RAND_CONST2))) * RAND_CONST3);
}

// 2D Noise function
float noise(vec2 st) {
    // Integer part
    vec2 cell = floor(st);
    // Fractional part
    vec2 local = fract(st);

    // Four corners in 2D of a tile
    float corner00 = random(cell);
    // Top-right corner
    float corner10 = random(cell + vec2(1.0, 0.0));
    // Top-left corner
    float corner01 = random(cell + vec2(0.0, 1.0));
    // Bottom-right corner
    float corner11 = random(cell + vec2(1.0, 1.0));

    // Smooth interpolation
    vec2 blend = local * local * (3.0 - 2.0 * local);

    // Mix the four corners
    return mix(corner00, corner10, blend.x)
    + (corner01 - corner00) * blend.y * (1.0 - blend.x)
    + (corner11 - corner10) * blend.x * blend.y;
}

// Fractal Brownian Motion
float fbm(vec2 st) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.2;

    // Loop of octaves
    for (int i = 0; i < 5; i++) {
        // Accumulate noise with increasing frequency and decreasing amplitude
        value += amplitude * noise(st * frequency);
        // Increase frequency and decrease amplitude
        st *= 2.0;
        amplitude *= 0.5;
        // Increase frequency and decrease amplitude
        frequency *= 1.1;
    }

    return value;
}

// Function to create a flame shape
float flameShape(vec2 position, float time) {

    // Create a teardrop flame base shape
    // Makes flame taller than wide
    float aspect = 1.7;

    // Center the flame horizontally
    vec2 flamePos = position - vec2(1, 0);

    // Make the flame shape wider at the bottom and narrower at the top
    flamePos.x *= 1.0 + flamePos.y * 0.8;

    // Calculate distance from center line, adjust for teardrop shape
    float dist = length(vec2(flamePos.x, flamePos.y / aspect));

    // Basic flame shape with bottom cutoff
    float flame = 0.35 - dist;

    // Cut off bottom half of the circle to make flame shape
    if (flamePos.y < -0.05) {
        flame -= (abs(flamePos.y) + 0.05) * 3.0;
    }

    // Add turbulence based on position and time
    float turb1 = fbm(vec2(flamePos.x * 4.0, flamePos.y * 4.0 + time * 1.5)) * 0.2;
    float turb2 = fbm(vec2(flamePos.x * 8.0 + sin(time * 0.3), flamePos.y * 2.0 + time)) * 0.1;

    // Add noise to flame edge for realism
    flame += turb1 - turb2;

    // Add some pulsating effect
    flame += 0.05 * sin(time * 3.0 + flamePos.y * 8.0);

    return flame;
}

void main() {
    if (isBorder == 1) {
        // Yellow border
        FragColor = vec4(1.0, 1.0, 0.0, 1.0);
        return;
    }

    // Normalize fragment position
    vec2 position = gl_FragCoord.xy / 400.0;

    // Get flame shape
    float flame = flameShape(position, time);

    // Colors for the flame
    // Deep red base
    vec3 baseColor = vec3(1.0, 0.1, 0.0);
    // Orange middle
    vec3 middleColor = vec3(1.0, 0.5, 0.0);
    // Yellow tip
    vec3 tipColor = vec3(1.0, 0.9, 0.0);
    // Bright inner flame
    vec3 innerColor = vec3(1.0, 0.9, 0.6);

    // Start with the base color
    vec3 color = baseColor;

    // Mix in mid flame color
    color = mix(color, middleColor, clamp(flame * 2.0, 0.0, 1.0));

    // Mix in tip color for the upper parts
    color = mix(color, tipColor, clamp(flame * 3.0 - 0.5, 0.0, 1.0));

    // Add bright inner core
    color = mix(color, innerColor, clamp(flame * 4.0 - 2.0, 0.0, 1.0));

    // Add some variation from noise
    float colorNoise = fbm(position * 5.0 + time * 0.1) * 0.1;
    color += colorNoise;

    // Set alpha based on flame intensity with a sharp cutoff for a natural edge
    float alpha = smoothstep(-0.05, 0.05, flame);

    // Add flicker to alpha as well
    alpha *= 0.9 + 0.1 * sin(time * 10.0 + position.y * 20.0);

    // Final color with alpha
    FragColor = vec4(color, alpha);
}
