#pragma once

#include "microcompute/microcompute.h"
#include "vector/vector.h"

/**
 * @brief The data for a camera
 */
typedef struct {
    vec3 pos;          ///< The position of the camera
    vec3 dir;          ///< The direction of the camera (LR, UD, _)
    vec2 sensorSize;   ///< The size of the camera sensor
    float focalLength; ///< The focal length of the camera
} CameraData;

typedef struct {
    CameraData data;       ///< The data for the camera
    mc_Buffer_t* dataBuff; ///< The GPU buffer for the camera data
} Camera;

/**
 * @brief Create a new camera
 *
 * @param device The device to create the camera on
 * @param sensorSize The size of the camera sensor
 * @param focalLength The focal length of the camera
 * @return A new camera
 */
Camera* camera_create(mc_Device_t* device, vec2 sensorSize, float focalLength);

/**
 * @brief Destroy a camera
 *
 * @param camera The camera to destroy
 */
void camera_destroy(Camera* camera);

/**
 * @brief Upload the camera data to the GPU
 *
 * @param camera The camera to update
 */
void camera_update(Camera* camera);

/**
 * @brief Get the position and direction of a camera
 *
 * @param camera The camera to get
 * @param pos The position of the camera
 * @param dir The direction of the camera in degrees (LR, UD, _)
 */
void camera_get(Camera* camera, vec3* pos, vec3* dir);

/**
 * @brief Set the position and direction of a camera
 *
 * @param camera The camera to set
 * @param pos The new position of the camera
 * @param dir The new direction of the camera in degrees (LR, UD, _)
 */
void camera_set(Camera* camera, vec3 pos, vec3 dir);