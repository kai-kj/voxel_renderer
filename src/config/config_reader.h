#pragma once

#include "renderer/renderer.h"

void read_config(
    char* fileName,
    mc_Device_t* device,
    char** outputFile,
    RenderSettings* info,
    Scene** scene,
    Camera** camera
);