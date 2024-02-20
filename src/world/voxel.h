#pragma once

#include "vec_types/vec_types.h"

/**
 * @brief A voxel
 */
typedef struct {
    uint d0; ///< Type and color of the voxel
    uint d1; ///< Other properties of the voxel
} Voxel;

/**
 * @brief Create an empty voxel
 * @return An empty voxel
 */
Voxel voxel_empty();

/**
 * @brief Create a filled voxel
 *
 * @param r The red component of the color
 * @param g The green component of the color
 * @param b The blue component of the color
 * @param emission The emission strength of the voxel
 * @return The packed voxel
 */
Voxel voxel(float r, float g, float b, float emission);
