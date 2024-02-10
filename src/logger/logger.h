#pragma once

#include <stdio.h>

#include "microcompute/microcompute.h"

#define LOG(lvl, ...) new_log(lvl, "vr", NULL, __FILE__, __LINE__, __VA_ARGS__)

#define DEBUG(...) LOG(MC_LOG_LEVEL_DEBUG, __VA_ARGS__)
#define INFO(...) LOG(MC_LOG_LEVEL_INFO, __VA_ARGS__)
#define WARN(...) LOG(MC_LOG_LEVEL_WARN, __VA_ARGS__)
#define ERROR(...) LOG(MC_LOG_LEVEL_ERROR, __VA_ARGS__)

void set_log_level(mc_LogLevel_t lvl);

void new_log(
    mc_LogLevel_t lvl,
    const char* src,
    void* arg,
    const char* file,
    int line,
    const char* fmt,
    ...
);