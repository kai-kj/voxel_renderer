#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "logger/logger.h"
#include "scene.h"

typedef struct {
    uvec3 size;
    Material bg;
} SceneData;

struct Scene {
    SceneData data;
    uint materialCapacity;
    uint materialCount;
    Material* materials;
    uint* voxels;
    mc_Buffer_t* dataBuff;
    mc_Buffer_t* materialBuff;
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

Scene* scene_create(mc_Device_t* device, SceneCreateInfo sceneCreateInfo) {
    CHECK_NULL(device, NULL)
    INFO("creating scene");

    Scene* scene = malloc(sizeof *scene);
    *scene = (Scene){
        .data = {.size = sceneCreateInfo.size, .bg = sceneCreateInfo.bg},
        .materialCapacity = 10,
        .materialCount = 1,
    };

    scene->materials = malloc(sizeof(Material) * scene->materialCapacity);

    scene->voxels = malloc(voxels_size(scene));
    for (uint i = 0; i < voxels_count(scene); i++) scene->voxels[i] = 0;

    scene->dataBuff
        = mc_buffer_create_from(device, sizeof(SceneData), &scene->data);
    scene->materialBuff
        = mc_buffer_create(device, sizeof(Material) * scene->materialCapacity);
    scene->voxelBuff
        = mc_buffer_create_from(device, voxels_size(scene), scene->voxels);

    return scene;
}

void scene_destroy(Scene* scene) {
    CHECK_NULL(scene)
    DEBUG("destroying scene");

    free(scene->materials);
    free(scene->voxels);
    mc_buffer_destroy(scene->dataBuff);
    mc_buffer_destroy(scene->materialBuff);
    mc_buffer_destroy(scene->voxelBuff);
    free(scene);
}

void scene_update_data(Scene* scene) {
    CHECK_NULL(scene)
    INFO("updating scene data");
    mc_buffer_write(scene->dataBuff, 0, sizeof scene->data, &scene->data);
}

void scene_update_materials(Scene* scene) {
    CHECK_NULL(scene)
    INFO("updating scene materials");
    mc_buffer_write(
        scene->materialBuff,
        0,
        sizeof *scene->materials * scene->materialCount,
        scene->materials
    );
}

void scene_update_voxels(Scene* scene) {
    CHECK_NULL(scene)
    INFO("updating scene voxels");
    mc_buffer_write(scene->voxelBuff, 0, voxels_size(scene), scene->voxels);
}

uint scene_register_material(Scene* scene, Material material) {
    CHECK_NULL(scene, 0)
    if (scene->materialCount == scene->materialCapacity) {
        scene->materialCapacity *= 2;
        scene->materials = realloc(
            scene->materials,
            sizeof *scene->materials * scene->materialCapacity
        );
        scene->materialBuff = mc_buffer_realloc(
            scene->materialBuff,
            sizeof *scene->materials * scene->materialCapacity
        );
    }

    DEBUG("registering material %d", scene->materialCount);

    scene->materials[scene->materialCount++] = material;
    return scene->materialCount - 1;
}

void scene_set(Scene* scene, uvec3 pos, uint materialID) {
    CHECK_NULL(scene)
    if (!coord_in_bounds(scene, pos)) return;
    scene->voxels[coord_to_index(scene, pos)] = materialID;
}

mc_Buffer_t* scene_get_data_buff(Scene* scene) {
    CHECK_NULL(scene, NULL)
    return scene->dataBuff;
}

mc_Buffer_t* scene_get_material_buff(Scene* scene) {
    CHECK_NULL(scene, NULL)
    return scene->materialBuff;
}

mc_Buffer_t* scene_get_voxel_buff(Scene* scene) {
    CHECK_NULL(scene, NULL)
    return scene->voxelBuff;
}