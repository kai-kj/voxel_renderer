#pragma once

#include <stdint.h>

typedef enum {
    VOXEL_TYPE_EMPTY,
    VOXEL_TYPE_LAMBERT,
} VoxelType;

typedef struct {
    uint32_t d0;
    uint32_t d1;
} Voxel;

Voxel voxel_pack(
    VoxelType type,
    float r,
    float g,
    float b,
    float p0,
    float p1,
    float p2,
    float p3
);

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
);

#define VOXEL_EMPTY() voxel_pack(VOXEL_TYPE_EMPTY, 0, 0, 0, 0, 0, 0, 0)

#define VOXEL_LAMBERT(r, g, b)                                                 \
    voxel_pack(VOXEL_TYPE_LAMBERT, r, g, b, 0, 0, 0, 0)
