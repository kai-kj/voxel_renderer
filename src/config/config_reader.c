#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lua5.3/lauxlib.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>

#include "config_reader.h"
#include "generic_parser.h"
#include "logger/logger.h"

#define LUA_SET_FIELD(L, key, type, value)                                     \
    lua_pushstring(L, key);                                                    \
    lua_push##type(L, value);                                                  \
    lua_settable(L, -3)

static void raise_error(lua_State* L, const char* fmt) {
    lua_pushstring(L, fmt);
    lua_error(L);
}

static int l_scene_register_material(lua_State* L) {
    lua_getuservalue(L, 1);
    Scene* scene = lua_touserdata(L, -1);
    lua_pop(L, 1);

    if (!scene) raise_error(L, "invalid scene");

    vec3 color;
    float emission;
    bool res = parse(
        L,
        "{color: {1: f, 2: f, 3: f}, emission: f}",
        &color.r,
        &color.g,
        &color.b,
        &emission
    );

    if (!res) raise_error(L, "invalid material");

    Material m = material(color, emission);
    lua_pushinteger(L, scene_register_material(scene, m));
    return 1;
}

static int l_scene_set(lua_State* L) {
    lua_getuservalue(L, 1);
    Scene* scene = lua_touserdata(L, -1);
    lua_pop(L, 1);

    if (!scene) raise_error(L, "invalid scene");

    vec3 pos;
    int materialID;
    bool res = parse(
        L,
        "i; {1: f, 2: f, 3: f}",
        &materialID,
        &pos.x,
        &pos.y,
        &pos.z
    );

    if (!res) raise_error(L, "invalid position or material");
    scene_set(scene, (uvec3){.x = pos.x, .y = pos.y, .z = pos.z}, materialID);
    return 0;
}

bool read_config(
    char* fileName,
    mc_Device_t* device,
    char** outputFile,
    RenderSettings* info,
    Scene** scene,
    Camera** camera
) {
    INFO("reading config file \"%s\n\"", fileName);

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    if (luaL_dofile(L, fileName)) {
        ERROR(
            "failed to run config file \"%s\": %s\n",
            fileName,
            lua_tostring(L, -1)
        );
        lua_close(L);
        return false;
    }

    int dataFunction;
    float bgEmission, cameraFocalLength;
    vec2 cameraSensorSize;
    vec3 bgColor, cameraPosition, cameraRotation;
    uvec3 sceneSize;

    char* format = "{"
                   "    output_file: s,"
                   "    settings: {"
                   "        renderer_code: s,"
                   "        output_code: s,"
                   "        workgroup_size: {1: i, 2: i},"
                   "        image_size: {1: i, 2: i},"
                   "        iterations: i,"
                   "        max_depth: i"
                   "    },"
                   "    scene: {"
                   "        size: {1: i, 2: i, 3: i},"
                   "        bg: {color: {1: f, 2: f, 3: f}, emission: f},"
                   "        data: l"
                   "    },"
                   "    camera: {"
                   "        sensor_size: {1: f, 2: f},"
                   "        focal_length: f,"
                   "        position: {1: f, 2: f, 3: f},"
                   "        rotation: {1: f, 2: f, 3: f}"
                   "    }"
                   "}";

    bool res = parse(
        L,
        format,
        outputFile,
        &info->rendererCode,
        &info->outputCode,
        &info->wgSize.x,
        &info->wgSize.y,
        &info->imageSize.x,
        &info->imageSize.y,
        &info->iters,
        &info->maxRayDepth,
        &sceneSize.x,
        &sceneSize.y,
        &sceneSize.z,
        &bgColor.r,
        &bgColor.g,
        &bgColor.b,
        &bgEmission,
        &dataFunction,
        &cameraSensorSize.x,
        &cameraSensorSize.y,
        &cameraFocalLength,
        &cameraPosition.x,
        &cameraPosition.y,
        &cameraPosition.z,
        &cameraRotation.x,
        &cameraRotation.y,
        &cameraRotation.z
    );

    if (!res) {
        ERROR("failed to parse config file \"%s\"\n", fileName);
        lua_close(L);
        return false;
    }

    *scene = scene_create(device, sceneSize, material(bgColor, bgEmission));
    *camera = camera_create(device, cameraSensorSize, cameraFocalLength);
    camera_set(*camera, cameraPosition, cameraRotation);

    INFO("running lua scene data function\n");
    lua_rawgeti(L, LUA_REGISTRYINDEX, dataFunction);

    lua_createtable(L, 0, 0);

    lua_pushstring(L, "size");
    lua_createtable(L, 0, 0);
    LUA_SET_FIELD(L, "x", number, scene_get_size(*scene).x);
    LUA_SET_FIELD(L, "y", number, scene_get_size(*scene).y);
    LUA_SET_FIELD(L, "z", number, scene_get_size(*scene).z);
    lua_settable(L, -3); // size

    LUA_SET_FIELD(L, "register_material", cfunction, l_scene_register_material);
    LUA_SET_FIELD(L, "set", cfunction, l_scene_set);

    lua_pushlightuserdata(L, *scene);
    lua_setuservalue(L, -2);

    if (lua_pcall(L, 1, 0, 0)) {
        ERROR("error in lua function: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        lua_close(L);
        return false;
    }

    lua_close(L);
    return true;
}