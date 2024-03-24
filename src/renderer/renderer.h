#pragma once

#include "world/camera.h"
#include "world/scene.h"

typedef struct {
    char* rendererCode;  ///< The renderer shader code
    char* iterationCode; ///< The iteration shader code
    char* outputCode;    ///< The output shader code
    uvec2 wgSize;        ///< The workgroup size
    uvec2 imageSize;     ///< The size of the image
    uint iterations;     ///< The number of iterations
    uint maxRayDepth;    ///< The maximum ray depth
} RenderSettings;

/**
 * @brief Render a scene
 * @param dev The device to render with
 * @param settings The settings for the render
 * @param scene The scene to render
 * @param camera The camera to render from
 * @return The rendered image on success, NULL on failure (must be freed by the
 * caller)
 */
unsigned char* render(
    mc_Device* dev,
    RenderSettings settings,
    Scene* scene,
    Camera* camera
);
