#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "logger/logger.h"
#include "scene.h"

typedef struct {
    uvec3 size;
    vec4 bgColor;
} SceneData;

struct Scene {
    SceneData data;
    Voxel* voxels;
    mc_Buffer_t* dataBuff;
    mc_Buffer_t* voxelBuff;
};

static uint voxels_count(Scene* scene) {
    return scene->data.size.x * scene->data.size.y * scene->data.size.z;
}

static uint voxels_size(Scene* scene) {
    return sizeof *scene->voxels * voxels_count(scene);
}

static bool coord_in_bounds(Scene* scene, uvec3 pos) {
    return pos.x < scene->data.size.x && pos.y < scene->data.size.y
        && pos.z < scene->data.size.z;
}

static uint coord_to_index(Scene* scene, uvec3 pos) {
    return pos.z * scene->data.size.x * scene->data.size.y
         + pos.y * scene->data.size.x + pos.x;
}

Scene* scene_create(
    mc_Device_t* device,
    uvec3 size,
    vec3 bgColor,
    float bgEmission
) {
    INFO("creating scene");

    Scene* scene = malloc(sizeof *scene);
    *scene = (Scene){
        .data = {
            .size = size,
            .bgColor = (vec4){bgColor.r, bgColor.g, bgColor.b, bgEmission},
        },
    };

    scene->voxels = malloc(voxels_size(scene));
    for (uint i = 0; i < voxels_count(scene); i++)
        scene->voxels[i] = voxel_empty();

    scene->dataBuff
        = mc_buffer_create_from(device, sizeof scene->data, &scene->data);
    scene->voxelBuff
        = mc_buffer_create_from(device, voxels_size(scene), scene->voxels);

    return scene;
}

void scene_destroy(Scene* scene) {
    INFO("destroying scene");

    free(scene->voxels);
    mc_buffer_destroy(scene->dataBuff);
    mc_buffer_destroy(scene->voxelBuff);
    free(scene);
}

void scene_update_data(Scene* scene) {
    INFO("updating scene data");
    mc_buffer_write(scene->dataBuff, 0, sizeof scene->data, &scene->data);
}

void scene_update_voxels(Scene* scene) {
    INFO("updating scene voxels");
    mc_buffer_write(scene->voxelBuff, 0, voxels_size(scene), scene->voxels);
}

uvec3 scene_get_size(Scene* scene) {
    return scene->data.size;
}

void scene_set(Scene* scene, uvec3 pos, Voxel voxel) {
    if (!coord_in_bounds(scene, pos)) return;
    scene->voxels[coord_to_index(scene, pos)] = voxel;
}

mc_Buffer_t* scene_get_data_buff(Scene* scene) {
    return scene->dataBuff;
}

mc_Buffer_t* scene_get_voxel_buff(Scene* scene) {
    return scene->voxelBuff;
}