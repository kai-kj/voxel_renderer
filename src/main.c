#include <stdio.h>

#include "renderer/renderer.h"
#include "stb/stb_image_write.h"

int main(int argc, char** argv) {
    uvec3 sceneSize = {100, 100, 100};
    uvec2 imageSize = {500, 500};
    char* shaderPath = "renderer.spv";

    mc_Instance_t* instance = mc_instance_create(mc_default_debug_cb, NULL);
    mc_Device_t* dev = mc_instance_get_devices(instance)[0];

    Renderer* renderer = renderer_create(dev, imageSize, shaderPath);
    Scene* scene = scene_create(dev, sceneSize, (vec3){0, 0, 0});
    Camera* camera = camera_create(dev);

    char* image = renderer_render(renderer, scene, camera, 100);
    stbi_write_bmp("mandelbrot.bmp", imageSize.x, imageSize.y, 4, image);

    renderer_destroy(renderer);
    scene_destroy(scene);
    camera_destroy(camera);

    mc_instance_destroy(instance);
    return 0;
}