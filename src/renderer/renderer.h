#pragma once

#include "world/camera.h"
#include "world/scene.h"

typedef struct {
    uvec2 imageSize;  ///< The size of the image
    uint iters;       ///< The number of iterations
    uint maxRayDepth; ///< The maximum ray depth
} RenderSettings;

/**
 * @brief Render a scene
 *
 * @param dev The device to render with
 * @param rendererShaderPath The path to the renderer shader
 * @param outputShaderPath The path to the output shader
 * @param settings The settings for the render
 * @param scene The scene to render
 * @param camera The camera to render from
 * @return The rendered image on success, NULL on failure (must be freed by the
 * caller)
 */
unsigned char* render(
    mc_Device_t* dev,
    char* rendererShaderPath,
    char* outputShaderPath,
    RenderSettings settings,
    Scene* scene,
    Camera* camera
);
