#include "world/camera.h"
#include "world/scene.h"

/**
 * @brief The data for the renderer
 */
typedef struct {
    uint maxRayDepth; ///< The maximum ray depth
    uint iteration;   ///< The current iteration
    float seed;       ///< The seed for the renderer
} RendererInfo;

/**
 * @brief A renderer
 */
typedef struct {
    RendererInfo info;      ///< The info for the renderer
    mc_Program_t* program;  ///< The program for the renderer
    uvec2 imageSize;        ///< The size of the image to render
    char* image;            ///< The image data
    mc_Buffer_t* infoBuff;  ///< The buffer for the info
    mc_Buffer_t* imageBuff; ///< The buffer for the image
} Renderer;

/**
 * @brief Create a new renderer
 *
 * @param dev The device to create the renderer on
 * @param imageSize The size of the image to render
 * @param rendererShaderPath The path to the renderer shader
 * @return A new renderer
 */
Renderer* renderer_create(
    mc_Device_t* dev,
    uvec2 imageSize,
    char* rendererShaderPath,
    uint maxRayDepth
);

/**
 * @brief Destroy a renderer
 *
 * @param renderer The renderer to destroy
 */
void renderer_destroy(Renderer* renderer);

/**
 * @brief Render a scene
 *
 * @param renderer The renderer to use
 * @param scene The scene to render
 * @param camera The camera to render from
 * @param iterations The number of iterations to render for
 * @return The rendered image
 */
char* renderer_render(
    Renderer* renderer,
    Scene* scene,
    Camera* camera,
    uint32_t iterations
);
