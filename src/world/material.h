#pragma once

#define MC_VEC_SHORT_NAMES
#include "microcompute/mc_vec.h"

typedef struct {
    vec3 color;
    vec4 properties;
} Material;

Material material(vec3 color, float emission);