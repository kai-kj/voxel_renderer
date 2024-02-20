#include <stdio.h>

#include "logger/logger.h"
#include "renderer/renderer.h"
#include "stb/stb_image_write.h"

char* OUT_FILE = "out.bmp";
char* RENDERER_PATH = "renderer.spv";
char* OUTPUT_PATH = "output.spv";
uvec3 SCENE_SZ = {25, 25, 25};
uvec2 IMAGE_SZ = {1920, 1080};

void create_cornell_box(Scene* scene) {
    Voxel white = voxel(0.8, 0.8, 0.8, 0.0);
    Voxel red = voxel(0.8, 0.0, 0.0, 0.0);
    Voxel green = voxel(0.0, 0.8, 0.0, 0.0);
    Voxel light = voxel(1.0, 1.0, 0.8, 0.9);

    // floor and ceiling
    for (uint i = 0; i < SCENE_SZ.x; i++) {
        for (uint j = 0; j < SCENE_SZ.z; j++) {
            scene_set(scene, (uvec3){i, 0, j}, white);
            scene_set(scene, (uvec3){i, SCENE_SZ.y - 1, j}, white);
        }
    }

    // left and right walls
    for (uint i = 0; i < SCENE_SZ.y; i++) {
        for (uint j = 0; j < SCENE_SZ.z; j++) {
            scene_set(scene, (uvec3){0, i, j}, red);
            scene_set(scene, (uvec3){SCENE_SZ.x - 1, i, j}, green);
        }
    }

    // back wall
    for (uint i = 0; i < SCENE_SZ.x; i++) {
        for (uint j = 0; j < SCENE_SZ.y; j++) {
            scene_set(scene, (uvec3){i, j, SCENE_SZ.z - 1}, white);
        }
    }

    // cube
    for (uint i = 0; i < SCENE_SZ.x / 5; i++) {
        for (uint j = 0; j < SCENE_SZ.y / 3; j++) {
            for (uint k = 0; k < SCENE_SZ.z / 5; k++) {
                scene_set(
                    scene,
                    (uvec3){
                        SCENE_SZ.x / 5 + i,
                        SCENE_SZ.y - j,
                        SCENE_SZ.z / 5 + k,
                    },
                    white
                );
            }
        }
    }

    // light
    for (uint i = 0; i < SCENE_SZ.x / 2; i++) {
        for (uint j = 0; j < SCENE_SZ.z / 2; j++) {
            scene_set(
                scene,
                (uvec3){
                    SCENE_SZ.x / 4 + i,
                    0,
                    SCENE_SZ.z / 4 + j,
                },
                light
            );
        }
    }
}

int main(int argc, char** argv) {
    set_log_level(MC_LOG_LEVEL_DEBUG);

    INFO("creating microcompute instance");
    mc_Instance_t* instance = mc_instance_create(new_log, NULL);
    mc_Device_t* dev = mc_instance_get_devices(instance)[0];

    Renderer* renderer
        = renderer_create(dev, IMAGE_SZ, RENDERER_PATH, OUTPUT_PATH, 10);
    Scene* scene = scene_create(dev, SCENE_SZ, (vec3){0.75, 0.75, 1.0}, 0.5);
    Camera* camera = camera_create(dev, (vec2){1.6, 0.9}, 1);

    create_cornell_box(scene);

    camera_set(
        camera,
        (vec3){SCENE_SZ.x * 0.75, SCENE_SZ.y * 0.25, -(int)SCENE_SZ.z * 1.2},
        (vec3){5, 10, 0}
    );

    scene_update_data(scene);
    scene_update_voxels(scene);
    camera_update(camera);

    char* image = renderer_render(renderer, scene, camera, 25);

    INFO("writing image to %s", OUT_FILE);
    stbi_write_bmp(OUT_FILE, IMAGE_SZ.x, IMAGE_SZ.y, 4, image);

    renderer_destroy(renderer);
    scene_destroy(scene);
    camera_destroy(camera);

    INFO("destroying microcompute instance");
    mc_instance_destroy(instance);

    return 0;
}