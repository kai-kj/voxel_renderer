#pragma once

#include <stdint.h>

typedef enum {
    VOXEL_EMPTY,
    VOXEL_LAMBERT,
} VoxelType;

typedef struct {
    uint32_t d0;
    uint32_t d1;
} Voxel;

Voxel voxel_pack(
    VoxelType type,
    uint8_t r,
    uint8_t g,
    uint8_t b,
    uint8_t p0,
    uint8_t p1,
    uint8_t p2,
    uint8_t p3
);

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
);

#define VOXEL_EMPTY() voxel_pack(VOXEL_EMPTY, 0, 0, 0, 0, 0, 0, 0)

#define VOXEL_LAMBERT(r, g, b) voxel_pack(VOXEL_LAMBERT, 0, 0, 0, r, g, b, 0)