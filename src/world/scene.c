#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "scene.h"

static uint32_t voxels_count(Scene* scene) {
    return scene->data.size.x * scene->data.size.y * scene->data.size.z;
}

static uint32_t voxels_size(Scene* scene) {
    return sizeof *scene->voxels * voxels_count(scene);
}

static bool coord_in_bounds(Scene* scene, uvec3 pos) {
    return pos.x < scene->data.size.x && pos.y < scene->data.size.y
        && pos.z < scene->data.size.z;
}

static uint32_t coord_to_index(Scene* scene, uvec3 pos) {
    return pos.z * scene->data.size.x * scene->data.size.y
         + pos.y * scene->data.size.x + pos.x;
}

Scene* scene_create(mc_Device_t* device, uvec3 size, vec3 bgColor) {
    Scene* scene = malloc(sizeof *scene);
    *scene = (Scene){.data = {.size = size, .bgColor = bgColor}};

    scene->voxels = malloc(voxels_size(scene));
    for (uint32_t i = 0; i < voxels_count(scene); i++)
        scene->voxels[i] = VOXEL_EMPTY();

    scene->dataBuff
        = mc_buffer_create_from(device, sizeof scene->data, &scene->data);
    scene->voxelBuff
        = mc_buffer_create_from(device, voxels_size(scene), scene->voxels);

    return scene;
}

void scene_destroy(Scene* scene) {
    free(scene->voxels);
    mc_buffer_destroy(scene->dataBuff);
    mc_buffer_destroy(scene->voxelBuff);
    free(scene);
}

void scene_update_data(Scene* scene) {
    mc_buffer_write(scene->dataBuff, 0, sizeof scene->data, &scene->data);
}

void scene_update_voxels(Scene* scene) {
    mc_buffer_write(scene->voxelBuff, 0, voxels_size(scene), scene->voxels);
}

void scene_set(Scene* scene, uvec3 pos, Voxel voxel) {
    if (!coord_in_bounds(scene, pos)) return;
    scene->voxels[coord_to_index(scene, pos)] = voxel;
}

Voxel scene_get(Scene* scene, uvec3 pos) {
    if (!coord_in_bounds(scene, pos)) return VOXEL_EMPTY();
    return scene->voxels[coord_to_index(scene, pos)];
}