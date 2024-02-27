#include <math.h>
#include <stdlib.h>

#include "camera.h"
#include "logger/logger.h"

#define PI 3.14159265358979323846

typedef struct {
    vec3 pos;
    vec3 dir;
    vec2 sensorSize;
    float focalLength;
} CameraData;

struct Camera {
    CameraData data;
    mc_Buffer_t* dataBuff;
};

float deg2rad(float deg) {
    return deg * PI / 180;
}

float rad2deg(float rad) {
    return rad * 180 / PI;
}

Camera* camera_create(mc_Device_t* device, vec2 sensorSize, float focalLength) {
    INFO("creating camera");

    Camera* camera = malloc(sizeof *camera);
    *camera = (Camera) {
        .data = {
            .pos = {0, 0, 0},
            .dir = {0, 0, 0},
            .sensorSize = sensorSize,
            .focalLength = focalLength,
        },
    };

    camera->dataBuff
        = mc_buffer_create_from(device, sizeof camera->data, &camera->data);
    return camera;
}

void camera_destroy(Camera* camera) {
    INFO("destroying camera");

    mc_buffer_destroy(camera->dataBuff);
    free(camera);
}

void camera_update(Camera* camera) {
    INFO("updating camera");
    mc_buffer_write(camera->dataBuff, 0, sizeof camera->data, &camera->data);
}

void camera_get(Camera* camera, vec3* pos, vec3* dir) {
    *pos = camera->data.pos;
    *dir = (vec3){
        rad2deg(camera->data.dir.x),
        rad2deg(camera->data.dir.y),
        rad2deg(camera->data.dir.z),
    };
}

void camera_set(Camera* camera, vec3 pos, vec3 dir) {
    camera->data.pos = pos;
    camera->data.dir = (vec3){deg2rad(dir.x), deg2rad(dir.y), deg2rad(dir.z)};
}

mc_Buffer_t* camera_get_data_buff(Camera* camera) {
    return camera->dataBuff;
}