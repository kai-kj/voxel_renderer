#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "logger/logger.h"
#include "renderer.h"

#ifndef WORKGROUP_SIZE_X
#define WORKGROUP_SIZE_X 1
#endif

#ifndef WORKGROUP_SIZE_Y
#define WORKGROUP_SIZE_Y 1
#endif

Renderer* renderer_create(
    mc_Device_t* dev,
    uvec2 imageSize,
    char* rendererShaderPath,
    char* outputShaderPath,
    uint maxRayDepth
) {
    INFO("creating renderer");

    uint32_t maxWGSizeTotal = mc_device_get_max_workgroup_size_total(dev);
    uint32_t* maxWGSizeShape = mc_device_get_max_workgroup_size_shape(dev);

    if (WORKGROUP_SIZE_X * WORKGROUP_SIZE_Y > maxWGSizeTotal) {
        ERROR(
            "total workgroup size (%d) too large, max: %d",
            WORKGROUP_SIZE_X * WORKGROUP_SIZE_Y,
            maxWGSizeTotal
        );
        return NULL;
    }

    if (WORKGROUP_SIZE_X > maxWGSizeShape[0]) {
        ERROR(
            "workgroup size x (%d) too large, max: %d",
            WORKGROUP_SIZE_X,
            maxWGSizeShape[0]
        );
        return NULL;
    }

    if (WORKGROUP_SIZE_Y > maxWGSizeShape[1]) {
        ERROR(
            "workgroup size y (%d) too large, max: %d",
            WORKGROUP_SIZE_Y,
            maxWGSizeShape[1]
        );
        return NULL;
    }

    Renderer* renderer = malloc(sizeof *renderer);
    *renderer = (Renderer){
        .info = {maxRayDepth, 0, 0},
        .renderProgram = mc_program_create(dev, rendererShaderPath, "main"),
        .outputProgram = mc_program_create(dev, outputShaderPath, "main"),
        .imageSize = imageSize,
        .image = malloc(imageSize.x * imageSize.y * 4),
    };

    renderer->infoBuff = mc_buffer_create(dev, sizeof renderer->info);
    renderer->fImageBuff
        = mc_buffer_create(dev, imageSize.x * imageSize.y * sizeof(vec3));
    renderer->iImageBuff
        = mc_buffer_create(dev, imageSize.x * imageSize.y * sizeof(int));

    bool failed = false;
    if (!renderer->renderProgram) failed = true;
    if (!renderer->outputProgram) failed = true;

    return !failed ? renderer : NULL;
}

void renderer_destroy(Renderer* renderer) {
    INFO("destroying renderer");

    if (renderer->infoBuff) {
        mc_buffer_destroy(renderer->infoBuff);
        renderer->infoBuff = NULL;
    }

    if (renderer->fImageBuff) {
        mc_buffer_destroy(renderer->fImageBuff);
        renderer->fImageBuff = NULL;
    }

    if (renderer->iImageBuff) {
        mc_buffer_destroy(renderer->iImageBuff);
        renderer->iImageBuff = NULL;
    }

    if (renderer->image) {
        free(renderer->image);
        renderer->image = NULL;
    }

    if (renderer->renderProgram) {
        mc_program_destroy(renderer->renderProgram);
        renderer->renderProgram = NULL;
    }

    if (renderer->outputProgram) {
        mc_program_destroy(renderer->outputProgram);
        renderer->outputProgram = NULL;
    }

    free(renderer);
}

char* renderer_render(
    Renderer* renderer,
    Scene* scene,
    Camera* camera,
    uint32_t iterations
) {
    INFO("rendering %d iteration(s)", iterations);
    INFO("- WORK_GROUP_SIZE_X: %d", WORKGROUP_SIZE_X);
    INFO("- WORK_GROUP_SIZE_Y: %d", WORKGROUP_SIZE_Y);

    double start = mc_get_time();
    srand((uint)start);

    for (uint32_t i = 0; i < iterations; i++) {
        renderer->info.iteration = i + 1;
        renderer->info.seed = (float)rand() / (float)RAND_MAX;

        INFO(
            "rendering iteration %d/%d (%f%%)",
            i + 1,
            iterations,
            (i + 1) / (float)iterations * 100
        );

        mc_buffer_write(
            renderer->infoBuff,
            0,
            sizeof renderer->info,
            &renderer->info
        );

        mc_program_run(
            renderer->renderProgram,
            renderer->imageSize.x / WORKGROUP_SIZE_X,
            renderer->imageSize.y / WORKGROUP_SIZE_Y,
            1,
            renderer->infoBuff,
            renderer->fImageBuff,
            scene->dataBuff,
            scene->voxelBuff,
            camera->dataBuff
        );
    }

    double elapsed = mc_get_time() - start;
    INFO("rendered %d iterations in %f seconds", iterations, elapsed);

    mc_program_run(
        renderer->outputProgram,
        renderer->imageSize.x / WORKGROUP_SIZE_X,
        renderer->imageSize.y / WORKGROUP_SIZE_Y,
        1,
        renderer->fImageBuff,
        renderer->iImageBuff
    );

    mc_buffer_read(
        renderer->iImageBuff,
        0,
        renderer->imageSize.x * renderer->imageSize.y * 4,
        renderer->image
    );

    return renderer->image;
}