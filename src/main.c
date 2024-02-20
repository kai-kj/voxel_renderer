#include <stdio.h>

#include "logger/logger.h"
#include "renderer/renderer.h"
#include "stb/stb_image_write.h"

char* OUT_FILE = "out.bmp";
char* SHADER_PATH = "renderer.spv";
uvec3 SCENE_SIZE = {50, 50, 50};
uvec2 IMAGE_SIZE = {1920, 1080};

void create_cornell_box(Scene* scene) {
    Voxel white = VOXEL_LAMBERT(0.8, 0.8, 0.8);
    Voxel red = VOXEL_LAMBERT(0.8, 0.0, 0.0);
    Voxel green = VOXEL_LAMBERT(0.0, 0.8, 0.0);

    // floor and ceiling
    for (uint i = 0; i < SCENE_SIZE.x; i++) {
        for (uint j = 0; j < SCENE_SIZE.z; j++) {
            scene_set(scene, (uvec3){i, 0, j}, white);
            scene_set(scene, (uvec3){i, SCENE_SIZE.y - 1, j}, white);
        }
    }

    // left and right walls
    for (uint i = 0; i < SCENE_SIZE.y; i++) {
        for (uint j = 0; j < SCENE_SIZE.z; j++) {
            scene_set(scene, (uvec3){0, i, j}, red);
            scene_set(scene, (uvec3){SCENE_SIZE.x - 1, i, j}, green);
        }
    }

    // back wall
    for (uint i = 0; i < SCENE_SIZE.x; i++) {
        for (uint j = 0; j < SCENE_SIZE.y; j++) {
            scene_set(scene, (uvec3){i, j, SCENE_SIZE.z - 1}, white);
        }
    }

    // cube
    for (uint i = 0; i < SCENE_SIZE.x / 10; i++) {
        for (uint j = 0; j < SCENE_SIZE.y / 5; j++) {
            for (uint k = 0; k < SCENE_SIZE.z / 10; k++) {
                scene_set(
                    scene,
                    (uvec3){
                        i + SCENE_SIZE.x / 4,
                        SCENE_SIZE.y - j,
                        k + SCENE_SIZE.z / 4},
                    white
                );
            }
        }
    }
}

int main(int argc, char** argv) {
    set_log_level(MC_LOG_LEVEL_DEBUG);

    INFO("creating microcompute instance");
    mc_Instance_t* instance = mc_instance_create(new_log, NULL);
    mc_Device_t* dev = mc_instance_get_devices(instance)[0];

    Renderer* renderer = renderer_create(dev, IMAGE_SIZE, SHADER_PATH, 10);
    Scene* scene = scene_create(dev, SCENE_SIZE, (vec3){0, 0.8, 1.0}, 1.0);
    Camera* camera = camera_create(dev, (vec2){1.6, 0.9}, 1);

    create_cornell_box(scene);

    camera_set(
        camera,
        (vec3){SCENE_SIZE.x / 2, SCENE_SIZE.y / 2, -75},
        (vec3){0, 0, 0}
    );

    scene_update_data(scene);
    scene_update_voxels(scene);
    camera_update(camera);

    char* image = renderer_render(renderer, scene, camera, 1);

    INFO("writing image to %s", OUT_FILE);
    stbi_write_bmp(OUT_FILE, IMAGE_SIZE.x, IMAGE_SIZE.y, 4, image);

    renderer_destroy(renderer);
    scene_destroy(scene);
    camera_destroy(camera);

    INFO("destroying microcompute instance");
    mc_instance_destroy(instance);

    return 0;
}