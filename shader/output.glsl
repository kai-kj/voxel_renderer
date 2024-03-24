#version 430

layout (local_size_x = WORKGROUP_SIZE_X, local_size_y = WORKGROUP_SIZE_Y) in;

ivec2 glPos = ivec2(gl_GlobalInvocationID.xy);
ivec2 glSize = ivec2(gl_NumWorkGroups.xy * gl_WorkGroupSize.xy);

layout (std430, binding = 0) readonly buffer buff0 {
    vec3 floatImage[];
};

layout (std430, binding = 1) writeonly buffer buff1 {
    int byteImage[];
};

void main() {
    int idx = glPos.y * glSize.x + glPos.x;

    vec3 cf = floatImage[idx];
    ivec3 ci = clamp(ivec3(cf * 255), 0, 255);
    int cb = ci.r << 0 | ci.g << 8 | ci.b << 16 | 255 << 24;

    byteImage[idx] = cb;
}