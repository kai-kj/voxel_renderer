#pragma once

#define MC_VEC_SHORT_NAMES
#include "microcompute/mc.h"
#include "microcompute/mc_vec.h"

typedef struct Camera Camera;

typedef struct CameraCreateInfo {
    vec2 sensorSize;
    float focalLength;
    vec3 pos;
    vec3 rot;
} CameraCreateInfo;

/**
 * @brief Create a new camera
 * @param device The device to create the camera on
 * @param cameraCreateInfo The camera creation info
 * @return A new camera
 */
Camera* camera_create(mc_Device_t* device, CameraCreateInfo cameraCreateInfo);

/**
 * @brief Destroy a camera
 * @param camera The camera to destroy
 */
void camera_destroy(Camera* camera);

/**
 * @brief Upload the camera data to the GPU
 * @param camera The camera to update
 */
void camera_update(Camera* camera);

/**
 * @brief Set the position and rotation of a camera
 * @param camera The camera to set
 * @param pos The new position of the camera
 * @param rot The new rotation of the camera in degrees (LR, UD, _)
 */
void camera_set(Camera* camera, vec3 pos, vec3 rot);

/**
 * @brief Get the data buffer of a camera
 * @param camera The camera to get the data buffer of
 * @return The data buffer
 */
mc_Buffer_t* camera_get_data_buff(Camera* camera);