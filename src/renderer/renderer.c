#include "renderer.h"

#include <stdio.h>
#include <stdlib.h>

Renderer* renderer_create(
    uvec3 sceneSize,
    uvec2 imageSize,
    char* rendererShaderPath
) {
    Renderer* renderer = malloc(sizeof *renderer);
    *renderer = (Renderer){0};

    renderer->instance = mc_instance_create(mc_default_debug_cb, renderer);
    if (mc_instance_get_device_count(renderer->instance) == 0) {
        renderer_destroy(renderer);
        return NULL;
    }

    mc_Device_t* dev = mc_instance_get_devices(renderer->instance)[1];
    renderer->program = mc_program_create(dev, rendererShaderPath);
    renderer->scene = scene_create(dev, sceneSize, (vec3){0.0, 0.0, 0.0});
    renderer->camera = camera_create(dev);

    renderer->imageSize = imageSize;
    renderer->image = malloc(imageSize.x * imageSize.y * 4);
    renderer->imageBuff = mc_buffer_create(dev, imageSize.x * imageSize.y * 4);

    return renderer;
}

void renderer_destroy(Renderer* renderer) {
    if (!renderer) return;

    if (renderer->scene) {
        scene_destroy(renderer->scene);
        renderer->scene = NULL;
    }

    if (renderer->camera) {
        camera_destroy(renderer->camera);
        renderer->camera = NULL;
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

    if (renderer->instance) {
        mc_instance_destroy(renderer->instance);
        renderer->instance = NULL;
    }

    free(renderer);
}

char* renderer_render(Renderer* renderer, uint32_t iterations) {
    if (!renderer) return NULL;

    mc_program_setup(
        renderer->program,
        "main",
        0,
        renderer->imageBuff,
        renderer->scene->dataBuff,
        renderer->scene->voxelBuff,
        renderer->camera->dataBuff
    );

    for (uint32_t i = 0; i < iterations; i++) {
        mc_program_run(
            renderer->program,
            renderer->imageSize.x,
            renderer->imageSize.y,
            1,
            NULL
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