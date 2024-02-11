#include <stdio.h>

#include "logger/logger.h"
#include "renderer/renderer.h"
#include "stb/stb_image_write.h"

#define OUT_FILE "out.bmp"
#define SHADER_PATH "renderer.spv"

int main(int argc, char** argv) {
    set_log_level(MC_LOG_LEVEL_DEBUG);

    uvec3 sceneSize = {50, 50, 50};
    uvec2 imageSize = {500, 500};

    INFO("creating microcompute instance");
    mc_Instance_t* instance = mc_instance_create(new_log, NULL);
    mc_Device_t* dev = mc_instance_get_devices(instance)[0];

    Renderer* renderer = renderer_create(dev, imageSize, SHADER_PATH, 10);
    Scene* scene = scene_create(dev, sceneSize, (vec3){0, 0.8, 1.0}, 1.0);
    Camera* camera = camera_create(dev, (vec2){1, 1}, 1);

    camera_set(camera, (vec3){25, 30, 0}, (vec3){0, 0, 0});

    for (unsigned int i = 0; i < sceneSize.x; i++) {
        for (unsigned int j = 0; j < sceneSize.y; j++) {
            scene_set(scene, (uvec3){i, 40, j}, VOXEL_LAMBERT(1.0, 1.0, 1.0));
        }
    }

    scene_set(scene, (uvec3){25, 39, 25}, VOXEL_LAMBERT(1.0, 0.0, 0.0));

    scene_update_data(scene);
    scene_update_voxels(scene);
    camera_update(camera);

    char* image = renderer_render(renderer, scene, camera, 1);

    INFO("writing image to %s", OUT_FILE);
    stbi_write_bmp(OUT_FILE, imageSize.x, imageSize.y, 4, image);

    renderer_destroy(renderer);
    scene_destroy(scene);
    camera_destroy(camera);

    INFO("destroying microcompute instance");
    mc_instance_destroy(instance);

    return 0;
}