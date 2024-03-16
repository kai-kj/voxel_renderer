#pragma once

#include <stdbool.h>

#include <lua5.3/lauxlib.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>

/**
 * Format specifiers:
 * - b: boolean
 * - i: integer
 * - f: float
 * - s: string
 * - l: lua function (as a registry index int)
 * - u: userdata
 * - {<key>: <value>, <key>: <value>, ...}: table, where the values are format
 *   specifiers
 *
 * In order to retrieve multiple values from the stack separate the format
 * specifiers with ';'. The values are parsed from the top of the stack.
 */

/**
 * @brief Pop values from a lua_State
 * @param L  The lua_State to parse
 * @param fmt  The format string to use
 * @param  ... Pointers to the variables to store the values in
 * @return true if the parsing was successful, false otherwise
 */
bool lua_pop_f(lua_State* L, char* fmt, ...);

/**
 * @brief Push values to a lua_State
 * @param L The lua_State to write to
 * @param fmt The format string to use
 * @param ... The values to write
 * @return true if the writing was successful, false otherwise
 */
bool lua_push_f(lua_State* L, char* fmt, ...);

/**
 * @brief Raise an error in a lua_State
 * @param L The lua_State to raise the error in
 * @param fmt The format string to use
 * @param ... The values to write
 */
void lua_raise_error(lua_State* L, const char* fmt, ...);