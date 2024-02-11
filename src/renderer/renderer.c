#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "logger/logger.h"
#include "renderer.h"

Renderer* renderer_create(
    mc_Device_t* dev,
    uvec2 imageSize,
    char* rendererShaderPath,
    unsigned int maxRayDepth
) {
    INFO("creating renderer");

    Renderer* renderer = malloc(sizeof *renderer);
    *renderer = (Renderer){
        .info = {maxRayDepth, 0, 0},
        .program = mc_program_create(dev, rendererShaderPath, "main"),
        .imageSize = imageSize,
        .image = malloc(imageSize.x * imageSize.y * 4),
    };

    renderer->infoBuff = mc_buffer_create(dev, sizeof renderer->info);
    renderer->imageBuff = mc_buffer_create(dev, imageSize.x * imageSize.y * 4);

    return mc_program_is_initialized(renderer->program) ? renderer : NULL;
}

void renderer_destroy(Renderer* renderer) {
    INFO("destroying renderer");

    if (renderer->infoBuff) {
        mc_buffer_destroy(renderer->infoBuff);
        renderer->infoBuff = NULL;
    }

    if (renderer->imageBuff) {
        mc_buffer_destroy(renderer->imageBuff);
        renderer->imageBuff = NULL;
    }

    if (renderer->image) {
        free(renderer->image);
        renderer->image = NULL;
    }

    if (renderer->program) {
        mc_program_destroy(renderer->program);
        renderer->program = NULL;
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

    double start = mc_get_time();
    srand((unsigned int)start);

    for (uint32_t i = 0; i < iterations; i++) {
        renderer->info.iteration = i;
        renderer->info.seed = (float)rand() / (float)RAND_MAX;

        mc_buffer_write(
            renderer->infoBuff,
            0,
            sizeof renderer->info,
            &renderer->info
        );

        mc_program_run(
            renderer->program,
            renderer->imageSize.x,
            renderer->imageSize.y,
            1,
            renderer->infoBuff,
            renderer->imageBuff,
            scene->dataBuff,
            scene->voxelBuff,
            camera->dataBuff
        );
    }

    double elapsed = mc_get_time() - start;
    INFO("rendered %d iterations in %f seconds", iterations, elapsed);

    mc_buffer_read(
        renderer->imageBuff,
        0,
        renderer->imageSize.x * renderer->imageSize.y * 4,
        renderer->image
    );

    return renderer->image;
}