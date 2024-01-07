#include <math.h>
#include <stdlib.h>

#include "camera.h"

Camera* camera_create(mc_Device_t* device) {
    Camera* camera = malloc(sizeof *camera);
    *camera = (Camera){.data = {.pos = {0, 0, 0}, .dir = {0, 0, 0}}};
    camera->dataBuff
        = mc_buffer_create_from(device, sizeof camera->data, &camera->data);
    return camera;
}

void camera_destroy(Camera* camera) {
    mc_buffer_destroy(camera->dataBuff);
    free(camera);
}

void camera_update(Camera* camera) {
    mc_buffer_write(camera->dataBuff, 0, sizeof camera->data, &camera->data);
}

void camera_set(Camera* camera, vec3 pos, vec3 dir) {
    camera->data = (CameraData){.pos = pos, .dir = dir};
}

void camera_move(Camera* camera, vec3 delta) {
    camera->data.pos = (vec3){
        camera->data.pos.x + delta.x,
        camera->data.pos.y + delta.y,
        camera->data.pos.z + delta.z,
    };
}

void camera_rotate(Camera* camera, vec3 delta) {
    camera->data.dir = (vec3){
        camera->data.dir.x + delta.x,
        camera->data.dir.y + delta.y,
        camera->data.dir.z + delta.z,
    };
}
