#version 430

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

//============================================================================//
// structs
//============================================================================//

struct Voxel {
    uint d0;
    uint d1;
};

struct SceneData {
    uvec3 size;
    vec3 bgColor;
};

struct CameraData {
    vec3 pos;
    vec3 dir;
};

//============================================================================//
// buffers
//============================================================================//

layout(std430, push_constant) uniform buffp {
    float seed;
};

layout(std430, binding = 0) buffer buff0 {
    int img[];
};

layout(std430, binding = 1) buffer buff1 {
    SceneData scene;
};

layout(std430, binding = 2) buffer buff2 {
    Voxel voxels[];
};

layout(std430, binding = 3) buffer buff3 {
    CameraData camera;
};

//============================================================================//
// rng
//============================================================================//

float prev = 0;

void init_rand(ivec2 pos, ivec2 size) {
    prev = pos.y * size.x + pos.x;
}

float rand() {
    prev = fract(sin(dot(vec2(prev, seed), vec2(12.989, 78.233))) * 43758.545);
    return prev;
}

//============================================================================//
// logic
//============================================================================//

int pack_color(vec3 color) {
    ivec3 c = clamp(ivec3(color * 255), 0, 255);
    return c.r << 0 | c.g << 8 | c.b << 16 | 255 << 24;
}

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = ivec2(gl_NumWorkGroups.xy);
    init_rand(pos, size);

    vec3 color = vec3(vec2(pos) / vec2(size), 0) + (1 - 2 * rand()) * 0.25;
    img[pos.y * size.x + pos.x] = pack_color(color);
}