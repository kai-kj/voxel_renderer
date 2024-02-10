#include "world/camera.h"
#include "world/scene.h"

typedef struct {
    mc_Program_t* program;
    uvec2 imageSize;
    char* image;
    mc_Buffer_t* seedBuff;
    mc_Buffer_t* imageBuff;
} Renderer;

Renderer* renderer_create(
    mc_Device_t* dev,
    uvec2 imageSize,
    char* rendererShaderPath
);

void renderer_destroy(Renderer* renderer);

char* renderer_render(
    Renderer* renderer,
    Scene* scene,
    Camera* camera,
    uint32_t iterations
);
