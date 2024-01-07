#include "voxel.h"

Voxel voxel_pack(
    VoxelType type,
    uint8_t r,
    uint8_t g,
    uint8_t b,
    uint8_t p0,
    uint8_t p1,
    uint8_t p2,
    uint8_t p3
) {
    return (Voxel){
        .d0 = (type << 24) | (r << 16) | (g << 8) | b,
        .d1 = (p0 << 24) | (p1 << 16) | (p2 << 8) | p3,
    };
}

void voxel_unpack(
    Voxel voxel,
    VoxelType* type,
    uint8_t* r,
    uint8_t* g,
    uint8_t* b,
    uint8_t* p0,
    uint8_t* p1,
    uint8_t* p2,
    uint8_t* p3
) {
    *type = (voxel.d0 >> 24) & 0xFF;
    *r = (voxel.d0 >> 16) & 0xFF;
    *g = (voxel.d0 >> 8) & 0xFF;
    *b = (voxel.d0 >> 0) & 0xFF;
    *p0 = (voxel.d1 >> 24) & 0xFF;
    *p1 = (voxel.d1 >> 16) & 0xFF;
    *p2 = (voxel.d1 >> 8) & 0xFF;
    *p3 = (voxel.d1 >> 0) & 0xFF;
}