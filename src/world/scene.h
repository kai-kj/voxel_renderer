#pragma once

#define MC_VEC_SHORT_NAMES
#include "microcompute/mc.h"
#include "microcompute/mc_vec.h"

#include "material.h"

typedef struct Scene Scene;

typedef struct SceneCreateInfo {
    uvec3 size;
    Material bg;
} SceneCreateInfo;

/**
 * @brief Create a new scene
 * @param device The device to create the scene on
 * @param sceneCreateInfo The scene creation info
 * @return A new scene
 */
Scene* scene_create(mc_Device_t* device, SceneCreateInfo sceneCreateInfo);

/**
 * @brief Destroy a scene
 * @param scene The scene to destroy
 */
void scene_destroy(Scene* scene);

/**
 * @brief Upload the scene data to the GPU
 * @param scene The scene to update
 */
void scene_update_data(Scene* scene);

/**
 * @brief Upload the scene materials to the GPU
 * @param scene The scene to update
 */
void scene_update_materials(Scene* scene);

/**
 * @brief Upload the scene voxels to the GPU
 * @param scene The scene to update
 */
void scene_update_voxels(Scene* scene);

/**
 * @brief Create a new material in a scene
 * @param scene The scene to create the material in
 * @param material The material to create
 * @return The ID of the created material
 */
uint scene_register_material(Scene* scene, Material material);

/**
 * @brief Set a voxel in a scene
 * @param scene The scene to set the voxel in
 * @param pos The position of the voxel
 * @param materialID The material ID of the voxel
 */
void scene_set(Scene* scene, uvec3 pos, uint materialID);

/**
 * @brief Get the data buffer of a scene
 * @param scene The scene to get the data buffer of
 * @return The data buffer
 */
mc_Buffer_t* scene_get_data_buff(Scene* scene);

/**
 * @brief Get the material buffer of a scene
 * @param scene The scene to get the material buffer of
 * @return The material buffer
 */
mc_Buffer_t* scene_get_material_buff(Scene* scene);

/**
 * @brief Get the voxel buffer of a scene
 * @param scene The scene to get the voxel buffer of
 * @return The voxel buffer
 */
mc_Buffer_t* scene_get_voxel_buff(Scene* scene);