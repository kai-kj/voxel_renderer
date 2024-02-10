#version 430

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

ivec2 glPos = ivec2(gl_GlobalInvocationID.xy);
ivec2 glSize = ivec2(gl_NumWorkGroups.xy);

//============================================================================//
// defines
//============================================================================//

#define VOXEL_TYPE_EMPTY 0
#define VOXEL_TYPE_LAMBERT 1

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

struct SceneData {
    uvec3 size;
    vec3 bgColor;
};

struct CameraData {
    vec3 pos;
    vec3 dir;
    vec3 sensorSize; // width, height, focal length
};

//============================================================================//
// buffers
//============================================================================//

layout(std430, binding = 0) buffer buff0 {
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
// logic
//============================================================================//

int color_pack(vec3 color) {
    ivec3 c = clamp(ivec3(color * 255), 0, 255);
    return c.r << 0 | c.g << 8 | c.b << 16 | 255 << 24;
}

Voxel get_voxel(uvec3 pos) {
    if (pos.x >= scene.size.x || pos.y >= scene.size.y
        || pos.z >= scene.size.z) {
        return Voxel(VOXEL_TYPE_EMPTY, vec3(0), vec4(0));
    }

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

// TODO: camera rotation
Ray generate_ray() {
    vec2 sensorPos
        = (vec2(glPos - glSize / 2) / vec2(glSize)) * camera.sensorSize.xy;
    vec3 dir = normalize(vec3(sensorPos, camera.sensorSize.z));
    return Ray(camera.pos, dir);
}

vec3 traverse(Ray ray) {
    vec3 tDelta = abs(1.0 / ray.dir);
    vec3 tMax = tDelta * (0.5 - mod(ray.origin, 1.0));

    ivec3 step = ivec3(sign(ray.dir));
    ivec3 pos = ivec3(ray.origin);

    while (true) {
        Voxel voxel = get_voxel(uvec3(pos));
        if (voxel.type != VOXEL_TYPE_EMPTY) return voxel.color;

        if (tMax.x < tMax.y) {
            if (tMax.x < tMax.z) {
                pos.x += step.x;
                tMax.x += tDelta.x;
                if (pos.x < 0 || pos.x >= scene.size.x) return vec3(0);
            } else {
                pos.z += step.z;
                tMax.z += tDelta.z;
                if (pos.z < 0 || pos.z >= scene.size.z) return vec3(0);
            }
        } else {
            if (tMax.y < tMax.z) {
                pos.y += step.y;
                tMax.y += tDelta.y;
                if (pos.y < 0 || pos.y >= scene.size.y) return vec3(0);
            } else {
                pos.z += step.z;
                tMax.z += tDelta.z;
                if (pos.z < 0 || pos.z >= scene.size.z) return vec3(0);
            }
        }
    }
}

void main() {
    // vec3 color = vec3(0.5 - generate_ray().dir.x * 0.5, 0, 0);

    Ray ray = generate_ray();
    vec3 color = traverse(ray);

    // vec3 color = 0.5 - dir * 0.5;
    // vec3 color = vec3(glPos / vec2(glSize), 1);
    // vec3 color = vec3(camera.sensorSize.z);

    img[glPos.y * glSize.x + glPos.x] = color_pack(color);
}