#include "world/camera.h"
#include "world/scene.h"

typedef struct {
    mc_Instance_t* instance;
    mc_Program_t* program;
    Scene* scene;
    Camera* camera;
    uvec2 imageSize;
    char* image;
    mc_Buffer_t* imageBuff;
} Renderer;

Renderer* renderer_create(
    uvec3 sceneSize,
    uvec2 imageSize,
    char* rendererShaderPath
);

void renderer_destroy(Renderer* renderer);

char* renderer_render(Renderer* renderer, uint32_t iterations);