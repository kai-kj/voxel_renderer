#include "config/config_reader.h"

#include <stdio.h>

#include "logger/logger.h"
#include "renderer/renderer.h"
#include "stb/stb_image_write.h"

char* RENDERER_PATH = "renderer.spv";
char* OUTPUT_PATH = "output.spv";

int main(int argc, char** argv) {
    set_log_level(MC_LOG_LEVEL_DEBUG);

    INFO("creating microcompute instance");
    mc_Instance_t* instance = mc_instance_create(new_log, NULL);
    mc_Device_t* dev = mc_instance_get_devices(instance)[1];

    INFO("using device \"%s\"", mc_device_get_name(dev));

    char* outputFile;
    RenderSettings settings;
    Scene* scene;
    Camera* camera;

    read_config(
        "../test/main.lua",
        dev,
        &outputFile,
        &settings,
        &scene,
        &camera
    );

    scene_update_data(scene);
    scene_update_voxels(scene);
    camera_update(camera);

    unsigned char* image
        = render(dev, RENDERER_PATH, OUTPUT_PATH, settings, scene, camera);

    INFO("writing image to %s", outputFile);
    stbi_write_bmp(
        outputFile,
        settings.imageSize.x,
        settings.imageSize.y,
        4,
        image
    );

    free(image);

    scene_destroy(scene);
    camera_destroy(camera);

    INFO("destroying microcompute instance");
    mc_instance_destroy(instance);

    return 0;
}