#include <math.h>

#include "voxel.h"

uint8_t f2b(float f) {
    return fmin(fmax(0.0, f), 1.0) * 255;
}

float b2f(uint8_t b) {
    return b / 255.0;
}

Voxel voxel_pack(
    VoxelType type,
    float r,
    float g,
    float b,
    float p0,
    float p1,
    float p2,
    float p3
) {
    return (Voxel){
        .d0 = (type << 24) | (f2b(r) << 16) | (f2b(g) << 8) | f2b(b),
        .d1 = (f2b(p0) << 24) | (f2b(p1) << 16) | (f2b(p2) << 8) | f2b(p3),
    };
}

void voxel_unpack(
    Voxel voxel,
    VoxelType* type,
    float* r,
    float* g,
    float* b,
    float* p0,
    float* p1,
    float* p2,
    float* p3
) {
    *type = (voxel.d0 >> 24) & 0xFF;
    *r = b2f((voxel.d0 >> 16) & 0xFF);
    *g = b2f((voxel.d0 >> 8) & 0xFF);
    *b = b2f((voxel.d0 >> 0) & 0xFF);
    *p0 = b2f((voxel.d1 >> 24) & 0xFF);
    *p1 = b2f((voxel.d1 >> 16) & 0xFF);
    *p2 = b2f((voxel.d1 >> 8) & 0xFF);
    *p3 = b2f((voxel.d1 >> 0) & 0xFF);
}