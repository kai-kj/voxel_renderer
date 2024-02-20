#include <math.h>
#include <stdbool.h>

#include "voxel.h"

unsigned char f2b(float f) {
    return fmin(fmax(0.0, f), 1.0) * 255;
}

Voxel voxel_pack(
    bool isEmpty,
    float r,
    float g,
    float b,
    float e,
    float p0,
    float p1,
    float p2
) {
    return (Voxel){
        .d0 = (isEmpty << 24) | (f2b(r) << 16) | (f2b(g) << 8) | f2b(b),
        .d1 = (f2b(e) << 24) | (f2b(p0) << 16) | (f2b(p1) << 8) | f2b(p2),
    };
}

Voxel voxel_empty() {
    return voxel_pack(true, 0, 0, 0, 0, 0, 0, 0);
}

Voxel voxel(float r, float g, float b, float emission) {
    return voxel_pack(false, r, g, b, emission, 0, 0, 0);
}
