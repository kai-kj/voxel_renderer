#include <stdio.h>

#include "renderer/renderer.h"
#include "stb/stb_image_write.h"

int main(int argc, char** argv) {
    uvec3 sceneSize = {100, 100, 100};
    uvec2 imageSize = {500, 500};
    char* shaderPath = "renderer.spv";

    Renderer* renderer = renderer_create(sceneSize, imageSize, shaderPath);

    char* image = renderer_render(renderer, 100);

    stbi_write_bmp("mandelbrot.bmp", imageSize.x, imageSize.y, 4, image);

    renderer_destroy(renderer);
    return 0;
}