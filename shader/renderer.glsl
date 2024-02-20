#version 430

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

ivec2 glPos = ivec2(gl_GlobalInvocationID.xy);
ivec2 glSize = ivec2(gl_NumWorkGroups.xy);

//============================================================================//
// defines
//============================================================================//

#define EPSILON 0.0001

#define VOXEL_TYPE_NONE 0
#define VOXEL_TYPE_EMPTY 1
#define VOXEL_TYPE_LIGHT 2
#define VOXEL_TYPE_LAMBERT 3
#define VOXEL_TYPE_METAL 4

//============================================================================//
// structs
//============================================================================//

struct Ray {
    vec3 origin;
    vec3 dir;
};

struct VoxelPacked {
    uint d0;
    uint d1;
};

struct Voxel {
    uint type;
    vec3 color;
    vec4 properties;
};

struct Hit {
    float dist;
    ivec3 norm;
    Voxel voxel;
};

struct SceneData {
    uvec3 size;
    vec3 bgColor;
    float bgIntensity;
};

struct CameraData {
    vec3 pos;
    vec3 dir;
    vec2 sensorSize;
    float focalLegnth;
};

//============================================================================//
// buffers
//============================================================================//

layout(std430, binding = 0) buffer buff0 {
    uint maxRayDepth;
    uint iteration;
    float seed;
};

layout(std430, binding = 1) buffer buff1 {
    int img[];
};

layout(std430, binding = 2) buffer buff2 {
    SceneData scene;
};

layout(std430, binding = 3) buffer buff3 {
    VoxelPacked voxels[];
};

layout(std430, binding = 4) buffer buff4 {
    CameraData camera;
};

//============================================================================//
// rng
//============================================================================//

float prev = glPos.y * glSize.x + glPos.x;

float rand() {
    prev = fract(sin(dot(vec2(prev, seed), vec2(12.989, 78.233))) * 43758.545);
    return prev;
}

//============================================================================//
// utility
//============================================================================//

#define VOXEL_NONE Voxel(VOXEL_TYPE_NONE, vec3(0), vec4(0))

#define EMPTY_HIT Hit(0, ivec3(0), VOXEL_NONE)

vec2 rotate(vec2 v, float a) {
    float s = sin(a);
    float c = cos(a);
    return vec2(v.x * c - v.y * s, v.x * s + v.y * c);
}

int color_pack(vec3 color) {
    ivec3 c = clamp(ivec3(color * 255), 0, 255);
    return c.r << 0 | c.g << 8 | c.b << 16 | 255 << 24;
}

Voxel get_voxel(uvec3 pos) {
    uint idx = (pos.z * scene.size.y + pos.y) * scene.size.x + pos.x;
    VoxelPacked packedData = voxels[idx];

    uint type = (packedData.d0 >> 24) & 0xFF;
    uint r = (packedData.d0 >> 16) & 0xFF;
    uint g = (packedData.d0 >> 8) & 0xFF;
    uint b = (packedData.d0 >> 0) & 0xFF;
    uint p0 = (packedData.d1 >> 24) & 0xFF;
    uint p1 = (packedData.d1 >> 16) & 0xFF;
    uint p2 = (packedData.d1 >> 8) & 0xFF;
    uint p3 = (packedData.d1 >> 0) & 0xFF;

    return Voxel(type, vec3(r, g, b) / 255.0, vec4(p0, p1, p2, p3) / 255.0);
}

bool in_scene_bounds(ivec3 pos) {
    return all(lessThanEqual(ivec3(0), pos)) && all(lessThan(pos, scene.size));
}

bool in_scene_bounds(vec3 pos) {
    return all(lessThanEqual(vec3(0), pos)) && all(lessThan(pos, scene.size));
}

//============================================================================//
// ray tracing
//============================================================================//

Ray generate_first_ray() {
    vec2 pos = (vec2(glPos - glSize / 2) / vec2(glSize)) * camera.sensorSize;
    pos = rotate(pos, camera.dir.z);

    vec3 dir = normalize(vec3(pos, camera.focalLegnth));
    dir.xz = rotate(dir.xz, camera.dir.x);
    dir.yz = rotate(dir.yz, camera.dir.y);

    return Ray(camera.pos, dir + vec3(EPSILON));
}

float ray_scene_intersection(Ray ray) {
    if (in_scene_bounds(ray.origin)) return 0;

    vec3 dirInv = 1.0 / ray.dir;
    vec3 tBottom = dirInv * (vec3(0) - ray.origin);
    vec3 tTop = dirInv * (scene.size - ray.origin);

    vec3 tMin = min(tTop, tBottom);
    vec3 tMax = max(tTop, tBottom);
    vec2 d = max(tMin.xx, tMin.yz);
    float dLow = max(d.x, d.y);
    d = min(tMax.xx, tMax.yz);
    float dHigh = min(d.x, d.y);

    return dHigh > max(dLow, 0.0) ? dLow : -1;
}

Hit traverse(Ray ray) {
    float d = ray_scene_intersection(ray);
    ray.origin += ray.dir * d * (1 + EPSILON);

    ivec3 pos = ivec3(floor(ray.origin));
    ivec3 step = ivec3(sign(ray.dir));

    vec3 tDelta = abs(length(ray.dir) / ray.dir);
    vec3 tMax
        = (sign(ray.dir) * (pos - ray.origin) + (sign(ray.dir) * 0.5) + 0.5)
        * tDelta;

    bvec3 mask = bvec3(false, false, false);

    while (true) {
        if (!in_scene_bounds(pos)) return EMPTY_HIT;

        Voxel voxel = get_voxel(uvec3(pos));

        if (voxel.type != VOXEL_TYPE_EMPTY)
            return Hit(
                length((tMax - tDelta) * vec3(mask)) + d,
                ivec3(mask) * step,
                voxel
            );

        if (tMax.x < tMax.y) {
            if (tMax.x < tMax.z) {
                tMax.x += tDelta.x;
                pos.x += step.x;
                mask = bvec3(true, false, false);
            } else {
                tMax.z += tDelta.z;
                pos.z += step.z;
                mask = bvec3(false, false, true);
            }
        } else {
            if (tMax.y < tMax.z) {
                tMax.y += tDelta.y;
                pos.y += step.y;
                mask = bvec3(false, true, false);
            } else {
                tMax.z += tDelta.z;
                pos.z += step.z;
                mask = bvec3(false, false, true);
            }
        }
    }
}

//============================================================================//
// main
//============================================================================//

void main() {
    Ray ray = generate_first_ray();

    Hit hit = traverse(ray);

    vec3 color;
    if (hit.voxel.type == VOXEL_TYPE_NONE) color = scene.bgColor;
    else color = hit.voxel.color;

    if (hit.norm.x != 0) color *= 0.9;
    if (hit.norm.y != 0) color *= 0.8;

    vec3 pos = ray.origin + ray.dir * hit.dist;

    // color = pos / scene.size;

    img[glPos.y * glSize.x + glPos.x] = color_pack(color);
}