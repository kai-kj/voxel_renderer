#include "renderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Renderer* renderer_create(
    mc_Device_t* dev,
    uvec2 imageSize,
    char* rendererShaderPath
) {
    Renderer* renderer = malloc(sizeof *renderer);
    *renderer = (Renderer){
        .program = mc_program_create(dev, rendererShaderPath),
        .imageSize = imageSize,
        .image = malloc(imageSize.x * imageSize.y * 4),
        .imageBuff = mc_buffer_create(dev, imageSize.x * imageSize.y * 4),
    };

    return mc_program_is_initialized(renderer->program) ? renderer : NULL;
}

void renderer_destroy(Renderer* renderer) {
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
    mc_program_setup(
        renderer->program,
        "main",
        sizeof(float),
        renderer->imageBuff,
        scene->dataBuff,
        scene->voxelBuff,
        camera->dataBuff
    );

    srand(time(NULL));

    for (uint32_t i = 0; i < iterations; i++) {
        float seed = (float)rand() / (float)RAND_MAX;
        mc_program_run(
            renderer->program,
            renderer->imageSize.x,
            renderer->imageSize.y,
            1,
            &seed
        );
    }

    mc_buffer_read(
        renderer->imageBuff,
        0,
        renderer->imageSize.x * renderer->imageSize.y * 4,
        renderer->image
    );

    return renderer->image;
}