#pragma once

#include "microcompute/microcompute.h"

#include "vec_types/vec_types.h"
#include "voxel.h"

typedef struct Scene Scene;

/**
 * @brief Create a new scene
 *
 * @param device The device to create the scene on
 * @param size The size of the scene
 * @param bgColor The background color of the scene
 * @param bgEmission The intensity/brightness of the background color
 * @return A new scene
 */
Scene* scene_create(
    mc_Device_t* device,
    uvec3 size,
    vec3 bgColor,
    float bgEmission
);

/**
 * @brief Destroy a scene
 *
 * @param scene The scene to destroy
 */
void scene_destroy(Scene* scene);

/**
 * @brief Upload the scene data to the GPU
 *
 * @param scene The scene to update
 */
void scene_update_data(Scene* scene);

/**
 * @brief Upload the scene voxels to the GPU
 *
 * @param scene The scene to update
 */
void scene_update_voxels(Scene* scene);

/**
 * @brief Get the size of a scene
 *
 * @param scene The scene to get the size of
 * @return The size of the scene
 */
uvec3 scene_get_size(Scene* scene);

/**
 * @brief Set a voxel in a scene
 *
 * @param scene The scene to set the voxel in
 * @param pos The position of the voxel
 * @param voxel The new voxel
 */
void scene_set(Scene* scene, uvec3 pos, Voxel voxel);

/**
 * @brief Get the data buffer of a scene
 *
 * @param scene The scene to get the data buffer of
 * @return The data buffer
 */
mc_Buffer_t* scene_get_data_buff(Scene* scene);

/**
 * @brief Get the voxel buffer of a scene
 *
 * @param scene The scene to get the voxel buffer of
 * @return The voxel buffer
 */
mc_Buffer_t* scene_get_voxel_buff(Scene* scene);