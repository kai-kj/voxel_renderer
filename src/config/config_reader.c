#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lua5.3/lauxlib.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>

#include "config_reader.h"
#include "logger/logger.h"

typedef void (*error_fn)(lua_State*, const char*, ...);

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

static void warn_error(lua_State* L, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    uint len = vsnprintf(NULL, 0, fmt, args);

    va_start(args, fmt);
    char* message = malloc(len + 1);
    vsnprintf(message, len + 1, fmt, args);
    va_end(args);

    WARN("lua: %s\n", message);
    free(message);
}

static void raise_error(lua_State* L, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    lua_pushvfstring(L, fmt, args);
    lua_error(L);
}

static void get_table(lua_State* L, const char* name, error_fn on_error) {
    lua_pushstring(L, name);
    lua_gettable(L, -2);
    if (!lua_istable(L, -1)) {
        on_error(L, "table \"%s\" not found", name);
        lua_pop(L, 1);
        lua_createtable(L, 0, 0);
        return;
    }
}

static float get_number(lua_State* L, const char* name, error_fn on_error) {
    lua_pushstring(L, name);
    lua_gettable(L, -2);
    if (!lua_isnumber(L, -1)) {
        on_error(L, "number \"%s\" not found, defaulting to 0", name);
        lua_pop(L, 1);
        return 0;
    }
    float value = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return value;
}

static float get_number_i(lua_State* L, int i, error_fn on_error) {
    lua_pushnumber(L, i);
    lua_gettable(L, -2);
    if (!lua_isnumber(L, -1)) {
        on_error(L, "number [%s] not found, defaulting to %g", i, 0.0);
        lua_pop(L, 1);
        return 0;
    }
    float value = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return value;
}

static char* get_string(lua_State* L, const char* name, error_fn on_error) {
    lua_pushstring(L, name);
    lua_gettable(L, -2);
    if (!lua_isstring(L, -1)) {
        on_error(L, "string \"%s\" not found, defaulting to NULL", name);
        lua_pop(L, 1);
        return NULL;
    }
    char* value = strdup(lua_tostring(L, -1));
    lua_pop(L, 1);
    return value;
}

static bool get_function(lua_State* L, const char* name, error_fn on_error) {
    lua_pushstring(L, name);
    lua_gettable(L, -2);
    if (!lua_isfunction(L, -1)) {
        on_error(L, "function \"%s\" not found\n", name);
        return false;
    }
    return true;
}

static int l_scene_register_material(lua_State* L) {
    lua_getuservalue(L, 1);
    Scene* scene = lua_touserdata(L, -1);
    lua_pop(L, 1);

    if (!scene) raise_error(L, "invalid scene");

    if (!lua_istable(L, 2)) raise_error(L, "invalid material");

    get_table(L, "color", raise_error);
    vec3 color = (vec3){
        .r = get_number_i(L, 1, raise_error),
        .g = get_number_i(L, 2, raise_error),
        .b = get_number_i(L, 3, raise_error),
    };
    lua_pop(L, 1); // color

    float emission = get_number(L, "emission", raise_error);

    lua_pushinteger(
        L,
        scene_register_material(scene, material(color, emission))
    );

    return 1;
}

static int l_scene_set(lua_State* L) {
    lua_getuservalue(L, 1);
    Scene* scene = lua_touserdata(L, -1);
    lua_pop(L, 1);

    if (!scene) raise_error(L, "invalid scene");

    if (!lua_isinteger(L, 3)) raise_error(L, "invalid material ID");
    uint materialID = lua_tointeger(L, 3);
    lua_pop(L, 1);

    if (!lua_istable(L, 2)) raise_error(L, "invalid position");

    uvec3 pos = (uvec3){
        .x = (int)get_number_i(L, 1, raise_error),
        .y = (int)get_number_i(L, 2, raise_error),
        .z = (int)get_number_i(L, 3, raise_error),
    };

    scene_set(scene, pos, materialID);

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

    lua_pushstring(L, "register_material");
    lua_pushcfunction(L, l_scene_register_material);
    lua_settable(L, -3); // register_material

    lua_pushstring(L, "set");
    lua_pushcfunction(L, l_scene_set);
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
            "failed to run config file \"%s\": %s\n",
            fileName,
            lua_tostring(L, -1)
        );
        lua_close(L);
        return;
    }

    *outputFile = get_string(L, "output_file", warn_error);

    get_table(L, "settings", warn_error);

    if (get_function(L, "renderer_code", warn_error)) {
        INFO("running renderer code gen function\n");
        if (lua_pcall(L, 0, 1, 0)) {
            WARN("error in lua function: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        } else if (!lua_isstring(L, -1)) {
            WARN("invalid renderer code, defaulting to empty string\n");
            info->rendererCode = strdup("");
        } else {
            info->rendererCode = strdup(lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }

    if (get_function(L, "output_code", warn_error)) {
        INFO("running output code gen function\n");
        if (lua_pcall(L, 0, 1, 0)) {
            WARN("error in lua function: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        } else if (!lua_isstring(L, -1)) {
            WARN("invalid output code, defaulting to empty string\n");
            info->outputCode = strdup("");
        } else {
            info->outputCode = strdup(lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }

    get_table(L, "workgroup_size", warn_error);
    info->wgSize.x = get_number_i(L, 1, warn_error);
    info->wgSize.y = get_number_i(L, 2, warn_error);
    lua_pop(L, 1); // workgroup_size

    get_table(L, "image_size", warn_error);
    info->imageSize.x = get_number_i(L, 1, warn_error);
    info->imageSize.y = get_number_i(L, 2, warn_error);
    lua_pop(L, 1); // image_size

    info->iters = get_number(L, "iterations", warn_error);
    info->maxRayDepth = get_number(L, "max_depth", warn_error);

    lua_pop(L, 1); // settings

    get_table(L, "scene", warn_error);

    get_table(L, "size", warn_error);
    uvec3 size = (uvec3){
        .x = get_number_i(L, 1, warn_error),
        .y = get_number_i(L, 2, warn_error),
        .z = get_number_i(L, 3, warn_error),
    };
    lua_pop(L, 1); // size

    get_table(L, "bg", warn_error);
    get_table(L, "color", warn_error);
    vec3 bg_color = (vec3){
        .r = get_number_i(L, 1, warn_error),
        .g = get_number_i(L, 2, warn_error),
        .b = get_number_i(L, 3, warn_error),
    };
    lua_pop(L, 1); // color
    float bg_emission = get_number(L, "emission", warn_error);
    lua_pop(L, 1); // bg

    *scene = scene_create(device, size, material(bg_color, bg_emission));

    if (get_function(L, "data", warn_error)) {
        INFO("running lua scene data function\n");
        make_scene_table(L, *scene);
        if (lua_pcall(L, 1, 0, 0)) {
            WARN("error in lua function: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }

    lua_pop(L, 1); // scene

    get_table(L, "camera", warn_error);

    get_table(L, "sensor_size", warn_error);
    vec2 sensorSize = (vec2){
        .x = get_number_i(L, 1, warn_error),
        .y = get_number_i(L, 2, warn_error),
    };
    lua_pop(L, 1); // sensor_size

    float focalLength = get_number(L, "focal_length", warn_error);

    get_table(L, "position", warn_error);
    vec3 position = (vec3){
        .x = get_number_i(L, 1, warn_error),
        .y = get_number_i(L, 2, warn_error),
        .z = get_number_i(L, 3, warn_error),
    };
    lua_pop(L, 1); // position

    get_table(L, "rotation", warn_error);
    vec3 direction = (vec3){
        .x = get_number_i(L, 1, warn_error),
        .y = get_number_i(L, 2, warn_error),
        .z = get_number_i(L, 3, warn_error),
    };
    lua_pop(L, 1); // direction

    *camera = camera_create(device, sensorSize, focalLength);
    camera_set(*camera, position, direction);

    lua_pop(L, 1); // camera
}