#include <stdio.h>

#include "config/config_reader.h"
#include "logger/logger.h"
#include "stb/stb_image_write.h"

int main(int argc, char** argv) {
    set_log_level(MC_LOG_LEVEL_DEBUG);

    INFO("creating microcompute instance");
    mc_Instance_t* instance = mc_instance_create(new_log, NULL);
    mc_Device_t* dev = mc_instance_get_devices(instance)[1];

    INFO("using device \"%s\"", mc_device_get_name(dev));

    char* outputFile = NULL;
    RenderSettings settings = {0};
    Scene* scene = NULL;
    Camera* camera = NULL;

    read_config(
        "../test/main.lua",
        dev,
        &outputFile,
        &settings,
        &scene,
        &camera
    );

    scene_update_data(scene);
    scene_update_materials(scene);
    scene_update_voxels(scene);
    camera_update(camera);

    unsigned char* image = render(dev, settings, scene, camera);

    INFO("writing image to \"%s\"", outputFile);
    stbi_write_bmp(
        outputFile,
        settings.imageSize.x,
        settings.imageSize.y,
        4,
        image
    );

    INFO("cleanup");
    free(image);
    scene_destroy(scene);
    camera_destroy(camera);
    mc_instance_destroy(instance);

    return 0;
}