#pragma once

#include "microcompute/microcompute.h"

#include "vec_types/vec_types.h"
#include "voxel.h"

/**
 * @brief The data for a scene
 */
typedef struct {
    uvec3 size;        ///< The size of the scene
    vec3 bgColor;      ///< The background color of the scene
    float bgIntensity; ///< The intensity/brightness of the background color
} SceneData;

/**
 * @brief A scene
 */
typedef struct {
    SceneData data;         ///< The data for the scene
    Voxel* voxels;          ///< The voxels in the scene
    mc_Buffer_t* dataBuff;  ///< The GPU buffer for the scene data
    mc_Buffer_t* voxelBuff; ///< The GPU buffer for the scene voxels
} Scene;

/**
 * @brief Create a new scene
 *
 * @param device The device to create the scene on
 * @param size The size of the scene
 * @param bgColor The background color of the scene
 * @param bgIntensity The intensity/brightness of the background color
 * @return A new scene
 */
Scene* scene_create(
    mc_Device_t* device,
    uvec3 size,
    vec3 bgColor,
    float bgIntensity
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
 * @brief Set a voxel in a scene
 *
 * @param scene The scene to set the voxel in
 * @param pos The position of the voxel
 * @param voxel The new voxel
 */
void scene_set(Scene* scene, uvec3 pos, Voxel voxel);

/**
 * @brief Get a voxel from a scene
 *
 * @param scene The scene to get the voxel from
 * @param pos The position of the voxel
 * @return The voxel
 */
Voxel scene_get(Scene* scene, uvec3 pos);