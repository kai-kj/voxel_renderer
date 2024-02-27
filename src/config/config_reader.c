#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lua5.3/lauxlib.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>

#include "config_reader.h"
#include "logger/logger.h"

__attribute__((used)) static void dump_stack(lua_State* L) {
    int top = lua_gettop(L);

    printf("stack (%d):\n", top);

    for (int i = 1; i <= top; i++) {
        printf("  %03d | %-10s | ", i, luaL_typename(L, i));

        switch (lua_type(L, i)) {
            case LUA_TNUMBER: printf("%f\n", lua_tonumber(L, i)); break;
            case LUA_TSTRING: printf("%s\n", lua_tostring(L, i)); break;
            case LUA_TBOOLEAN:
                printf("%s\n", lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNIL: printf("%s\n", "nil"); break;
            default: printf("%p\n", lua_topointer(L, i)); break;
        }
    }
}

static void get_table(lua_State* L, const char* name) {
    lua_pushstring(L, name);
    lua_gettable(L, -2);
    if (!lua_istable(L, -1)) {
        WARN("table \"%s\" not found\n", name);
        lua_pop(L, 1);
        lua_createtable(L, 0, 0);
        return;
    }
}

static float get_number(lua_State* L, const char* name) {
    lua_pushstring(L, name);
    lua_gettable(L, -2);
    if (!lua_isnumber(L, -1)) {
        WARN("number \"%s\" not found, defaulting to %g", name, 0.0);
        lua_pop(L, 1);
        return 0;
    }
    float value = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return value;
}

static char* get_string(lua_State* L, const char* name) {
    lua_pushstring(L, name);
    lua_gettable(L, -2);
    if (!lua_isstring(L, -1)) {
        WARN("string \"%s\" not found, defaulting to NULL", name);
        lua_pop(L, 1);
        return NULL;
    }
    char* value = strdup(lua_tostring(L, -1));
    lua_pop(L, 1);
    return value;
}

static bool get_function(lua_State* L, const char* name) {
    lua_pushstring(L, name);
    lua_gettable(L, -2);
    if (!lua_isfunction(L, -1)) {
        WARN("function \"%s\" not found\n", name);
        lua_pop(L, 1);
        return false;
    }
    return true;
}

static void return_error(lua_State* L, const char* message) {
    lua_pushstring(L, message);
    lua_error(L);
}

static int scene_set_voxel(lua_State* L) {
    lua_getuservalue(L, 1);
    Scene* scene = lua_touserdata(L, -1);
    lua_pop(L, 1);

    if (!scene) return_error(L, "invalid scene");

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3) || !lua_isnumber(L, 4))
        return_error(L, "invalid position");

    uvec3 pos = (uvec3){
        .x = (int)lua_tonumber(L, 2),
        .y = (int)lua_tonumber(L, 3),
        .z = (int)lua_tonumber(L, 4),
    };

    if (!lua_istable(L, 5)) return_error(L, "invalid voxel");

    float r = get_number(L, "r");
    float g = get_number(L, "g");
    float b = get_number(L, "b");
    float emission = get_number(L, "emission");

    scene_set(scene, pos, voxel(r, g, b, emission));

    return 0;
}

static void make_scene_table(lua_State* L, Scene* scene) {
    lua_createtable(L, 0, 0);

    lua_pushstring(L, "size");
    lua_createtable(L, 0, 0);

    lua_pushstring(L, "x");
    lua_pushnumber(L, scene_get_size(scene).x);
    lua_settable(L, -3); // x

    lua_pushstring(L, "y");
    lua_pushnumber(L, scene_get_size(scene).y);
    lua_settable(L, -3); // y

    lua_pushstring(L, "z");
    lua_pushnumber(L, scene_get_size(scene).z);
    lua_settable(L, -3); // z

    lua_settable(L, -3); // size

    lua_pushstring(L, "set");
    lua_pushcfunction(L, scene_set_voxel);
    lua_settable(L, -3); // set

    lua_pushlightuserdata(L, scene);
    lua_setuservalue(L, -2);
}

void read_config(
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
            "failed to run config file %s: %s\n",
            fileName,
            lua_tostring(L, -1)
        );
        lua_close(L);
        return;
    }

    *outputFile = get_string(L, "output_file");

    get_table(L, "settings");

    get_table(L, "image_size");
    info->imageSize.x = get_number(L, "x");
    info->imageSize.y = get_number(L, "y");
    lua_pop(L, 1); // image_size

    info->iters = get_number(L, "iterations");
    info->maxRayDepth = get_number(L, "max_depth");

    lua_pop(L, 1); // settings

    get_table(L, "scene");

    get_table(L, "size");
    uvec3 size = (uvec3){
        .x = get_number(L, "x"),
        .y = get_number(L, "y"),
        .z = get_number(L, "z"),
    };
    lua_pop(L, 1); // size

    get_table(L, "bg_color");
    vec3 bg_color = (vec3){
        .x = get_number(L, "r"),
        .y = get_number(L, "g"),
        .z = get_number(L, "b"),
    };
    lua_pop(L, 1); // bg_color

    float bg_intensity = get_number(L, "bg_intensity");

    *scene = scene_create(device, size, bg_color, bg_intensity);

    if (get_function(L, "data")) {
        INFO("running scene data function\n");
        make_scene_table(L, *scene);
        if (lua_pcall(L, 1, 0, 0)) {
            WARN("error in scene data function: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }

    lua_pop(L, 1); // scene

    get_table(L, "camera");

    get_table(L, "sensor_size");
    vec2 sensorSize = (vec2){
        .x = get_number(L, "x"),
        .y = get_number(L, "y"),
    };
    lua_pop(L, 1); // sensor_size

    float focalLength = get_number(L, "focal_length");

    get_table(L, "position");
    vec3 position = (vec3){
        .x = get_number(L, "x"),
        .y = get_number(L, "y"),
        .z = get_number(L, "z"),
    };
    lua_pop(L, 1); // position

    get_table(L, "rotation");
    vec3 direction = (vec3){
        .x = get_number(L, "x"),
        .y = get_number(L, "y"),
        .z = get_number(L, "z"),
    };
    lua_pop(L, 1); // direction

    *camera = camera_create(device, sensorSize, focalLength);
    camera_set(*camera, position, direction);

    lua_pop(L, 1); // camera
}