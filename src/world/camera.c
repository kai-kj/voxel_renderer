#include <stdlib.h>

#include "camera.h"
#include "logger/logger.h"

#define PI 3.14159265358979323846f

typedef struct {
    vec3 pos;
    vec3 dir;
    vec2 sensorSize;
    float focalLength;
} CameraData;

struct Camera {
    CameraData data;
    mce_HBuffer* dataBuff;
};

float deg2rad(float deg) {
    return deg * PI / 180;
}

float rad2deg(float rad) {
    return rad * 180 / PI;
}

Camera* camera_create(mc_Device* device, CameraCreateInfo cameraCreateInfo) {
    CHECK_NULL(device, NULL)
    INFO("creating camera");

    Camera* camera = malloc(sizeof *camera);
    *camera = (Camera) {
        .data = {
            .pos = {0, 0, 0},
            .dir = {0, 0, 0},
            .sensorSize = cameraCreateInfo.sensorSize,
            .focalLength = cameraCreateInfo.focalLength,
        },
    };

    camera->dataBuff = mce_hybrid_buffer_create_from(
        device,
        sizeof camera->data,
        &camera->data
    );

    camera_set(camera, cameraCreateInfo.pos, cameraCreateInfo.rot);

    return camera;
}

void camera_destroy(Camera* camera) {
    CHECK_NULL(camera)
    DEBUG("destroying camera");

    mce_hybrid_buffer_destroy(camera->dataBuff);
    free(camera);
}

void camera_update(Camera* camera) {
    CHECK_NULL(camera)
    INFO("updating camera");
    mce_hybrid_buffer_write(
        camera->dataBuff,
        0,
        sizeof camera->data,
        &camera->data
    );
}

void camera_set(Camera* camera, vec3 pos, vec3 dir) {
    CHECK_NULL(camera)
    camera->data.pos = pos;
    camera->data.dir = (vec3){deg2rad(dir.x), deg2rad(dir.y), deg2rad(dir.z)};
}

mce_HBuffer* camera_get_data_buff(Camera* camera) {
    CHECK_NULL(camera, NULL)
    return camera->dataBuff;
}