#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "logger/logger.h"
#include "lua_extra.h"

#define GET_TYPE(L) lua_typename(L, lua_type(L, -1))

static bool is_key(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool is_integer(const char* str, int len) {
    for (int i = 0; i < len; i++) {
        if (str[i] < '0' || str[i] > '9') return false;
    }
    return true;
}

static void consume_spaces(char** fmt) {
    while (**fmt == ' ') (*fmt)++;
}

static bool lua_pop_f__(lua_State* L, char** fmt, va_list arg) {
    while (true) {
        consume_spaces(fmt);

        switch (**fmt) {
            case 'b':
                if (!lua_isboolean(L, -1)) {
                    ERROR("expected boolean, got %s", GET_TYPE(L));
                    return false;
                }
                *va_arg(arg, bool*) = lua_toboolean(L, -1);
                break;
            case 'i':
                if (!lua_isinteger(L, -1)) {
                    ERROR("expected integer, got %s", GET_TYPE(L));
                    return false;
                }
                *va_arg(arg, int*) = lua_tointeger(L, -1);
                break;
            case 'f':
                if (!lua_isnumber(L, -1)) {
                    ERROR("expected number, got %s", GET_TYPE(L));
                    return false;
                }
                *va_arg(arg, float*) = lua_tonumber(L, -1);
                break;
            case 's':
                if (!lua_isstring(L, -1)) {
                    ERROR("expected string, got %s", GET_TYPE(L));
                    return false;
                }
                *va_arg(arg, const char**) = strdup(lua_tostring(L, -1));
                break;
            case 'l':
                if (!lua_isfunction(L, -1)) {
                    ERROR("expected function, got %s", GET_TYPE(L));
                    return false;
                }
                *va_arg(arg, int*) = luaL_ref(L, LUA_REGISTRYINDEX);
                lua_pushnil(L);
                break;
            case 'u':
                if (!lua_isuserdata(L, -1)) {
                    ERROR("expected userdata, got %s", GET_TYPE(L));
                    return false;
                }
                *va_arg(arg, void**) = lua_touserdata(L, -1);
                break;
            case '{':
                if (!lua_istable(L, -1)) {
                    ERROR("expected table, got %s", GET_TYPE(L));
                    return false;
                }

                (*fmt)++;
                while (true) {
                    consume_spaces(fmt);
                    char* key = *fmt;
                    while (is_key(**fmt)) (*fmt)++;
                    int keyLen = (int)(*fmt - key);
                    consume_spaces(fmt);
                    if (**fmt != ':') {
                        ERROR("expected ':', got '%c'", **fmt);
                        return false;
                    }
                    (*fmt)++;

                    if (is_integer(key, keyLen))
                        lua_pushinteger(L, strtol(key, NULL, 10));
                    else lua_pushlstring(L, key, keyLen);
                    lua_gettable(L, -2);

                    if (lua_isnil(L, -1)) {
                        ERROR("key '%.*s' not found", keyLen, key);
                        return false;
                    }

                    if (!lua_pop_f__(L, fmt, arg)) {
                        ERROR("  in '%.*s'", keyLen, key);
                        return false;
                    }

                    if (**fmt == ',') {
                        (*fmt)++;
                    } else if (**fmt == '}') {
                        break;
                    } else {
                        ERROR("expected ',' or '}', got '%c'", **fmt);
                        return false;
                    }
                }
                break;
            case '\0': return true;
            default:
                ERROR("expected on of \"bifslu{\" or '\0', got '%c'", **fmt);
                return false;
        }
        lua_pop(L, 1);

        (*fmt)++;
        consume_spaces(fmt);
        switch (**fmt) {
            case ';': (*fmt)++; continue;
            case ',':
            case '}':
            case '\0': return true;
            default:
                ERROR("expected ',', '}', ';' or '\0', got '%c'", **fmt);
                return false;
        }
    }
}

static bool lua_push_f__(lua_State* L, char** fmt, va_list arg) {
    while (true) {
        consume_spaces(fmt);

        switch (**fmt) {
            case 'b': lua_pushboolean(L, va_arg(arg, int)); break;
            case 'i': lua_pushinteger(L, va_arg(arg, int)); break;
            case 'f': lua_pushnumber(L, va_arg(arg, double)); break;
            case 's': lua_pushstring(L, va_arg(arg, const char*)); break;
            case 'l': lua_pushcfunction(L, va_arg(arg, lua_CFunction)); break;
            case 'u': lua_pushlightuserdata(L, va_arg(arg, void*)); break;
            case '{':
                lua_newtable(L);
                (*fmt)++;
                while (true) {
                    consume_spaces(fmt);
                    char* key = *fmt;
                    while (is_key(**fmt)) (*fmt)++;
                    int keyLen = (int)(*fmt - key);
                    consume_spaces(fmt);
                    if (**fmt != ':') {
                        ERROR("expected ':', got '%c'", **fmt);
                        return false;
                    }

                    (*fmt)++;

                    if (is_integer(key, keyLen)) {
                        lua_pushinteger(L, strtol(key, NULL, 10));
                    } else {
                        lua_pushlstring(L, key, keyLen);
                    }

                    if (!lua_push_f__(L, fmt, arg)) {
                        ERROR("  in '%.*s'", keyLen, key);
                        return false;
                    }

                    lua_settable(L, -3);

                    if (**fmt == ',') {
                        (*fmt)++;
                    } else if (**fmt == '}') {
                        break;
                    } else {
                        ERROR("expected ',' or '}', got '%c'", **fmt);
                        return false;
                    }
                }
                break;
            case '\0': return true;
            default:
                ERROR("expected on of \"bifslu{\" or '\0', got '%c'", **fmt);
                return false;
        }

        (*fmt)++;
        consume_spaces(fmt);
        switch (**fmt) {
            case ';': (*fmt)++; continue;
            case ',':
            case '}':
            case '\0': return true;
            default:
                ERROR("expected ',', '}', ';' or '\0', got '%c'", **fmt);
                return false;
        }
    }
}

bool lua_pop_f(lua_State* L, char* fmt, ...) {
    va_list arg;
    va_start(arg, fmt);
    bool res = lua_pop_f__(L, &fmt, arg);
    va_end(arg);
    return res;
}

bool lua_push_f(lua_State* L, char* fmt, ...) {
    va_list arg;
    va_start(arg, fmt);
    bool res = lua_push_f__(L, &fmt, arg);
    va_end(arg);
    return res;
}

void lua_raise_error(lua_State* L, const char* fmt, ...) {
    va_list arg;
    va_start(arg, fmt);
    lua_pushvfstring(L, fmt, arg);
    va_end(arg);
    lua_error(L);
}