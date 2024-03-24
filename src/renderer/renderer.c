#include <math.h>
#include <stdlib.h>

#include "logger/logger.h"
#include "renderer.h"
#include "shader_compiler.h"

typedef struct {
    uint maxRayDepth;
    uint iter;
    float seed;
} RenderInfo;

unsigned char* render(
    mc_Device* dev,
    RenderSettings settings,
    Scene* scene,
    Camera* camera
) {
    CHECK_NULL(dev, NULL)
    CHECK_NULL(scene, NULL)
    CHECK_NULL(camera, NULL)

    INFO("preparing render with settings:");
    INFO("- device: \"%s\"", mc_device_get_name(dev));
    INFO("- work group size: %dx%d", settings.wgSize.x, settings.wgSize.y);
    INFO("- image size: %dx%d", settings.imageSize.x, settings.imageSize.y);
    INFO("- iterations: %d", settings.iterations);
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

    INFO("updating scene and camera");
    scene_update_data(scene);
    scene_update_materials(scene);
    scene_update_voxels(scene);
    camera_update(camera);

    SPIRVCode renderCode = compile_glsl(
        "render_shader",
        settings.rendererCode,
        "main",
        settings.wgSize
    );
    if (renderCode.size == 0) {
        ERROR("failed to compile render code");
        return NULL;
    }

    mc_Program* renderProgram
        = mc_program_create(dev, renderCode.size, renderCode.code, "main");

    SPIRVCode iterCode = compile_glsl(
        "iteration_shader",
        settings.iterationCode,
        "main",
        (uvec2){1, 1}
    );
    if (iterCode.size == 0) {
        ERROR("failed to compile iteration code");
        return NULL;
    }

    mc_Program* iterProgram
        = mc_program_create(dev, iterCode.size, iterCode.code, "main");

    if (!iterProgram) {
        ERROR("failed to create iteration program");
        return NULL;
    }

    SPIRVCode outputCode = compile_glsl(
        "output_shader",
        settings.outputCode,
        "main",
        settings.wgSize
    );
    if (outputCode.size == 0) {
        ERROR("failed to compile output code");
        return NULL;
    }

    mc_Program* outputProgram
        = mc_program_create(dev, outputCode.size, outputCode.code, "main");

    if (!outputProgram) {
        ERROR("failed to create output program");
        return NULL;
    }

    mce_HBuffer* fImageBuff = mce_hybrid_buffer_create(
        dev,
        settings.imageSize.x * settings.imageSize.y * sizeof(vec3)
    );

    mce_HBuffer* iImageBuff = mce_hybrid_buffer_create(
        dev,
        settings.imageSize.x * settings.imageSize.y * sizeof(int)
    );

    mce_HBuffer* infoBuff = mce_hybrid_buffer_create(dev, sizeof(RenderInfo));

    INFO("starting render (%d iterations):", settings.iterations);
    double start = mc_get_time();

    RenderInfo info = {settings.maxRayDepth, 0, (float)(start - floor(start))};
    mce_hybrid_buffer_write(infoBuff, 0, sizeof info, &info);

    for (uint i = 0; i < settings.iterations; i++) {
        float progress = (float)(i + 1) / (float)settings.iterations * 100.0f;
        INFO("- %d/%d (%.2f%%)", i + 1, settings.iterations, progress);

        mc_program_run(iterProgram, 1, 1, 1, infoBuff);
        
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
    double rate = elapsed / settings.iterations;
    INFO(
        "finished render in %.02fs (%.02f ms/iteration)",
        elapsed,
        rate * 1000.0
    );

    INFO("converting image into bytes");

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

    mce_hybrid_buffer_read(
        iImageBuff,
        0,
        settings.imageSize.x * settings.imageSize.y * 4,
        image
    );

    DEBUG("cleaning up render");
    mc_program_destroy(renderProgram);
    mc_program_destroy(iterProgram);
    mc_program_destroy(outputProgram);
    mce_hybrid_buffer_destroy(infoBuff);
    mce_hybrid_buffer_destroy(fImageBuff);
    mce_hybrid_buffer_destroy(iImageBuff);

    return image;
}