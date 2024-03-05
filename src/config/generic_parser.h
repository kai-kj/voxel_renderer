#pragma once

#include <lua5.3/lua.h>
#include <stdbool.h>

/**
 * @brief Parse a lua_State and return the values in the stack
 * @param L  The lua_State to parse
 * @param fmt  The format string to use
 * @param  ... Pointers to the variables to store the values in
 * @return true if the parsing was successful, false otherwise
 *
 * Formatted in the following way:
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

bool parse(lua_State* L, char* fmt, ...);