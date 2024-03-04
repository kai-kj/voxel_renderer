#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "logger/logger.h"
#include "renderer.h"
#include "shader_code.h"

typedef struct {
    uint maxRayDepth;
    uint iter;
    float seed;
} RenderInfo;

unsigned char* render(
    mc_Device_t* dev,
    RenderSettings settings,
    Scene* scene,
    Camera* camera
) {
    INFO("preparing render with settings:");
    INFO("- device: \"%s\"", mc_device_get_name(dev));
    INFO("- work group size: %dx%d", settings.wgSize.x, settings.wgSize.y);
    INFO("- image size: %dx%d", settings.imageSize.x, settings.imageSize.y);
    INFO("- iters: %d", settings.iters);
    INFO("- max ray depth: %d", settings.maxRayDepth);

    uint maxWGSizeTotal = mc_device_get_max_workgroup_size_total(dev);
    uint* maxWGSizeShape = mc_device_get_max_workgroup_size_shape(dev);

    if (settings.wgSize.x * settings.wgSize.y > maxWGSizeTotal) {
        ERROR("total workgroup size  too large, max: %d", maxWGSizeTotal);
        return NULL;
    }

    if (settings.wgSize.x > maxWGSizeShape[0]) {
        ERROR("workgroup size x too large, max: %d", maxWGSizeShape[0]);
        return NULL;
    }

    if (settings.wgSize.y > maxWGSizeShape[1]) {
        ERROR("workgroup size y too large, max: %d", maxWGSizeShape[1]);
        return NULL;
    }

    ShaderCode* renderCode = shader_code_create(settings.rendererCode);
    if (!renderCode) {
        ERROR("failed to create render code");
        return NULL;
    }

    mc_Program_t* renderProgram = mc_program_create(
        dev,
        shader_code_get_size(renderCode),
        shader_code_get_bytecode(renderCode),
        "main"
    );

    shader_code_destroy(renderCode);

    if (!renderProgram) {
        ERROR("failed to create render program");
        return NULL;
    }

    ShaderCode* outputCode = shader_code_create(settings.outputCode);
    if (!outputCode) {
        ERROR("failed to create output code");
        return NULL;
    }

    mc_Program_t* outputProgram = mc_program_create(
        dev,
        shader_code_get_size(outputCode),
        shader_code_get_bytecode(outputCode),
        "main"
    );

    shader_code_destroy(outputCode);

    if (!outputProgram) {
        ERROR("failed to create output program");
        return NULL;
    }

    mc_Buffer_t* fImageBuff = mc_buffer_create(
        dev,
        settings.imageSize.x * settings.imageSize.y * sizeof(vec3)
    );

    mc_Buffer_t* iImageBuff = mc_buffer_create(
        dev,
        settings.imageSize.x * settings.imageSize.y * sizeof(int)
    );

    RenderInfo info = {settings.maxRayDepth, 0, 0};
    mc_Buffer_t* infoBuff = mc_buffer_create(dev, sizeof(RenderInfo));

    INFO("starting render (%d iters):", settings.iters);

    double start = mc_get_time();
    srand((uint)start);

    for (uint i = 0; i < settings.iters; i++) {
        info.iter = i + 1;
        info.seed = (float)rand() / (float)RAND_MAX;

        float progress = (float)(i + 1) / (float)settings.iters * 100.0f;
        INFO("- %d/%d (%.2f%%)", info.iter, settings.iters, progress);

        mc_buffer_write(infoBuff, 0, sizeof info, &info);

        mc_program_run(
            renderProgram,
            settings.imageSize.x / settings.wgSize.x,
            settings.imageSize.y / settings.wgSize.y,
            1,
            infoBuff,
            fImageBuff,
            scene_get_data_buff(scene),
            scene_get_material_buff(scene),
            scene_get_voxel_buff(scene),
            camera_get_data_buff(camera)
        );
    }

    double elapsed = mc_get_time() - start;
    double rate = elapsed / settings.iters;
    INFO("finished render in %.02fs (%.02f ms/frame)", elapsed, rate * 1000.0);

    INFO("convert image into bytes");

    unsigned char* image
        = malloc(settings.imageSize.x * settings.imageSize.y * 4);

    mc_program_run(
        outputProgram,
        settings.imageSize.x / settings.wgSize.x,
        settings.imageSize.y / settings.wgSize.y,
        1,
        fImageBuff,
        iImageBuff
    );

    mc_buffer_read(
        iImageBuff,
        0,
        settings.imageSize.x * settings.imageSize.y * 4,
        image
    );

    INFO("cleaning up render");
    mc_program_destroy(renderProgram);
    mc_program_destroy(outputProgram);
    mc_buffer_destroy(infoBuff);
    mc_buffer_destroy(fImageBuff);
    mc_buffer_destroy(iImageBuff);

    return image;
}