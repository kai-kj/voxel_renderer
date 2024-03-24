#version 430

layout (std430, binding = 0) coherent buffer buff0 {
    uint maxRayDepth;
    uint iteration;
    float seed;
};

void main() {
    iteration += 1;
    seed = fract(sin(dot(vec2(iteration) * seed, vec2(12.98, 78.23))) * 43758.54);
}