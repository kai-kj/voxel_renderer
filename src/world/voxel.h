#pragma once

#include <stdint.h>

/**
 * @brief The type of a voxel
 */
typedef enum {
    VOXEL_TYPE_EMPTY,   ///< An empty voxel
    VOXEL_TYPE_LAMBERT, ///< A Lambertian voxel
} VoxelType;

/**
 * @brief A voxel
 */
typedef struct {
    uint32_t d0; ///< Type and color of the voxel
    uint32_t d1; ///< Other properties of the voxel
} Voxel;

/**
 * @brief Pack a voxel
 *
 * @param type The type of the voxel
 * @param r The red component of the color
 * @param g The green component of the color
 * @param b The blue component of the color
 * @param p0 The first property of the voxel
 * @param p1 The second property of the voxel
 * @param p2 The third property of the voxel
 * @param p3 The fourth property of the voxel
 * @return The packed voxel
 */
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

/**
 * @brief Unpack a voxel
 *
 * @param voxel The voxel to unpack
 * @param type The type of the voxel
 * @param r The red component of the color
 * @param g The green component of the color
 * @param b The blue component of the color
 * @param p0 The first property of the voxel
 * @param p1 The second property of the voxel
 * @param p2 The third property of the voxel
 * @param p3 The fourth property of the voxel
 */
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

/**
 * @brief Create an empty voxel
 *
 * @return An empty voxel
 */
#define VOXEL_EMPTY() voxel_pack(VOXEL_TYPE_EMPTY, 0, 0, 0, 0, 0, 0, 0)

/**
 * @brief Create a Lambertian surface
 *
 * @param r The red component of the color (0 ~ 1)
 * @param g The green component of the color (0 ~ 1)
 * @param b The blue component of the color (0 ~ 1)
 * @return A Lambertian voxel
 */
#define VOXEL_LAMBERT(r, g, b)                                                 \
    voxel_pack(VOXEL_TYPE_LAMBERT, r, g, b, 0, 0, 0, 0)
