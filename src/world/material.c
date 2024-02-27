#include "material.h"

Material material(vec3 color, float emission) {
    return (Material){.color = color, .properties.x = emission};
}