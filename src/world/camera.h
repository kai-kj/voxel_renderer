#pragma once

#include "microcompute/microcompute.h"
#include "vector/vector.h"

typedef struct {
    vec3 pos;
    vec3 dir;
} CameraData;

typedef struct {
    CameraData data;
    mc_Buffer_t* dataBuff;
} Camera;

Camera* camera_create(mc_Device_t* device);

void camera_destroy(Camera* camera);

void camera_update(Camera* camera);

void camera_set(Camera* camera, vec3 pos, vec3 dir);

void camera_move(Camera* camera, vec3 delta);

void camera_rotate(Camera* camera, vec3 delta);
