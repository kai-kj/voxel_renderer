#pragma once

#include "microcompute/microcompute.h"

#include "vector/vector.h"
#include "voxel.h"

typedef struct {
    uvec3 size;
    vec3 bgColor;
} SceneData;

typedef struct {
    SceneData data;
    Voxel* voxels;
    mc_Buffer_t* dataBuff;
    mc_Buffer_t* voxelBuff;
} Scene;

Scene* scene_create(mc_Device_t* device, uvec3 size, vec3 bgColor);
void scene_destroy(Scene* scene);

void scene_update_data(Scene* scene);
void scene_update_voxels(Scene* scene);

void scene_set(Scene* scene, uvec3 pos, Voxel voxel);
Voxel scene_get(Scene* scene, uvec3 pos);