#version 430

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

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

void set_color(vec3 color) {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = ivec2(gl_NumWorkGroups.xy);
    ivec3 c = clamp(ivec3(color * 255), 0, 255);
    img[pos.y * size.x + pos.x] = c.r << 0 | c.g << 8 | c.b << 16 | 255 << 24;
}

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = ivec2(gl_NumWorkGroups.xy);
    set_color(vec3(vec2(pos) / vec2(size), 0));
}