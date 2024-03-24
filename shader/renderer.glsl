#version 430

layout (local_size_x = WORKGROUP_SIZE_X, local_size_y = WORKGROUP_SIZE_Y) in;

ivec2 glPos = ivec2(gl_GlobalInvocationID.xy);
ivec2 glSize = ivec2(gl_NumWorkGroups.xy * gl_WorkGroupSize.xy);

//============================================================================//
// defines
//============================================================================//

#define EPSILON 0.00001
#define PI 3.14159

//============================================================================//
// structs
//============================================================================//

struct Ray {
    vec3 origin;
    vec3 dir;
};

struct Material {
    vec3 color;
    vec3 properties;
};

struct Hit {
    float dist;
    ivec3 norm;
    uint material;
};

//============================================================================//
// buffers
//============================================================================//

layout (std430, binding = 0) readonly buffer buff0 {
    uint maxRayDepth;
    uint iteration;
    float seed;
};

layout (std430, binding = 1) coherent buffer buff1 {
    vec3 img[];
};

layout (std430, binding = 2) readonly buffer buff2 {
    uvec3 sceneSize;
    Material bg;
};

layout (std430, binding = 3) readonly buffer buff3 {
    Material materials[];
};

layout (std430, binding = 4) readonly buffer buff4 {
    uint voxels[];
};

layout (std430, binding = 5) readonly buffer buff5 {
    vec3 cameraPos;
    vec3 cameraDir;
    vec2 cameraSensorSize;
    float cameraFocalLegnth;
};

//============================================================================//
// rng
//============================================================================//

float prev = seed;

float rand() {
    prev = fract(sin(dot(vec2(glPos) * prev, vec2(12.98, 78.23))) * 43758.54);
    return prev;
}

float rand(float min, float max) {
    return rand() * (max - min) + min;
}

vec3 rand_unit() {
    float theta = rand(-PI / 2, PI / 2);
    float phi = rand(0, 2 * PI);
    return vec3(cos(theta) * cos(phi), cos(theta) * sin(phi), sin(theta));
}

//============================================================================//
// utility
//============================================================================//

vec2 rotate(vec2 v, float a) {
    float s = sin(a);
    float c = cos(a);
    return vec2(v.x * c - v.y * s, v.x * s + v.y * c);
}

bool in_scene_bounds(ivec3 pos) {
    return all(lessThanEqual(ivec3(0), pos)) && all(lessThan(pos, sceneSize));
}

bool in_scene_bounds(vec3 pos) {
    return all(lessThanEqual(vec3(0), pos)) && all(lessThan(pos, sceneSize));
}

Ray create_ray(vec3 origin, vec3 dir) {
    return Ray(origin, normalize(dir) + EPSILON);
}

uint get_voxel(uvec3 pos) {
    return voxels[(pos.z * sceneSize.y + pos.y) * sceneSize.x + pos.x];
}

//============================================================================//
// ray tracing
//============================================================================//

Ray generate_first_ray() {
    vec2 pos = (vec2(glPos - glSize / 2) / vec2(glSize)) * cameraSensorSize;
    pos = rotate(pos, cameraDir.z);

    vec3 dir = vec3(pos, cameraFocalLegnth);
    dir.xz = rotate(dir.xz, cameraDir.x);
    dir.yz = rotate(dir.yz, -cameraDir.y);

    return create_ray(cameraPos, dir);
}

float ray_scene_intersection(Ray ray) {
    if (in_scene_bounds(ray.origin)) return 0;

    vec3 dirInv = 1.0 / ray.dir;
    vec3 tBottom = dirInv * (vec3(0) - ray.origin);
    vec3 tTop = dirInv * (sceneSize - ray.origin);

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
    ray.origin += ray.dir * d * (1 - EPSILON);

    ivec3 pos = ivec3(floor(ray.origin));
    ivec3 step = ivec3(sign(ray.dir));

    vec3 tDelta = abs(length(ray.dir) / ray.dir);
    vec3 tMax = (sign(ray.dir) * (pos - ray.origin) + (sign(ray.dir) * 0.5) + 0.5) * tDelta;

    bvec3 mask = bvec3(false, false, false);

    while (true) {
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

        if (!in_scene_bounds(pos)) return Hit(0, ivec3(0), 0);

        uint materialID = get_voxel(uvec3(pos));

        if (materialID != 0) {
            float dist = length((tMax - tDelta) * vec3(mask)) + d;
            ivec3 norm = ivec3(mask) * -step;
            return Hit(dist, norm, materialID);
        }
    }
}

vec3 get_color(Ray ray) {
    vec3 throughput = vec3(1, 1, 1);

    for (int i = 0; i < maxRayDepth; i++) {
        Hit hit = traverse(ray);
        Material material = materials[hit.material];

        if (hit.norm == ivec3(0))
        return bg.color * bg.properties.x * throughput;

        if (material.properties.x > 0)
        return material.color * material.properties.x * throughput;

        throughput *= material.color;

        ray = create_ray(
            ray.origin + ray.dir * hit.dist + hit.norm * EPSILON,
            hit.norm + rand_unit()
        );
    }

    return vec3(0);
}

//============================================================================//
// main
//============================================================================//

void main() {
    vec3 color = get_color(generate_first_ray());
    vec3 oldColor = img[glPos.y * glSize.x + glPos.x];
    vec3 newColor = (oldColor * (iteration - 1) + color) / iteration;
    img[glPos.y * glSize.x + glPos.x] = newColor;
}