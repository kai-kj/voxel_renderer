#include <stdarg.h>
#include <string.h>

#include <lua5.3/lauxlib.h>
#include <lua5.3/lualib.h>

#include "generic_parser.h"
#include "logger/logger.h"

#define GET_TYPE(L) lua_typename(L, lua_type(L, -1))

static bool is_key(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool is_integer(char* str, int len) {
    for (int i = 0; i < len; i++) {
        if (str[i] < '0' || str[i] > '9') return false;
    }
    return true;
}

static void consume_spaces(char** fmt) {
    while (**fmt == ' ') (*fmt)++;
}

static bool parse_value(lua_State*, char**, va_list);

static bool parse_table(lua_State* L, char** fmt, va_list arg) {
    if (**fmt != '{') {
        ERROR("expected '{', got '%c'", **fmt);
        return false;
    }

    (*fmt)++;
    while (true) {
        consume_spaces(fmt);
        char* key = *fmt;
        while (is_key(**fmt)) (*fmt)++;
        int keyLen = *fmt - key;
        consume_spaces(fmt);
        if (**fmt != ':') {
            ERROR("expected ':', got '%c'", **fmt);
            return false;
        }
        (*fmt)++;

        if (is_integer(key, keyLen)) lua_pushinteger(L, strtol(key, NULL, 10));
        else lua_pushlstring(L, key, keyLen);
        lua_gettable(L, -2);

        if (lua_isnil(L, -1)) {
            ERROR("key '%.*s' not found", keyLen, key);
            return false;
        }

        if (!parse_value(L, fmt, arg)) {
            ERROR("  in '%.*s'", keyLen, key);
            return false;
        }

        switch (**fmt) {
            case ',': (*fmt)++; continue;
            case '}': return true;
            default:
                ERROR("expected ',' or '}', got '%c'", **fmt);
                return false;
        }
    }
}

static bool parse_value(lua_State* L, char** fmt, va_list arg) {
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
                if (!parse_table(L, fmt, arg)) return false;
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

bool parse(lua_State* L, char* fmt, ...) {
    va_list arg;
    va_start(arg, fmt);
    bool res = parse_value(L, &fmt, arg);
    va_end(arg);
    return res;
}