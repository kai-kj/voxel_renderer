#pragma once

#include <stdio.h>

#include "microcompute/microcompute.h"

/**
 * @brief Log a message
 *
 * @param lvl The level of the message
 * @param fmt The format of the message
 * @param ... The arguments for the message
 */
#define LOG(lvl, fmt, ...)                                                     \
    new_log(lvl, "vr", NULL, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief Log a message at the debug level
 *
 * @param fmt The format of the message
 * @param ... The arguments for the message
 */
#define DEBUG(fmt, ...) LOG(MC_LOG_LEVEL_DEBUG, fmt __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief Log a message at the info level
 *
 * @param fmt The format of the message
 * @param ... The arguments for the message
 */
#define INFO(fmt, ...) LOG(MC_LOG_LEVEL_INFO, fmt __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief Log a message at the warn level
 *
 * @param fmt The format of the message
 * @param ... The arguments for the message
 */
#define WARN(fmt, ...) LOG(MC_LOG_LEVEL_WARN, fmt __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief Log a message at the error level
 *
 * @param fmt The format of the message
 * @param ... The arguments for the message
 */
#define ERROR(fmt, ...) LOG(MC_LOG_LEVEL_ERROR, fmt __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief Set the log level
 *
 * @param lvl The new log level
 */
void set_log_level(mc_LogLevel_t lvl);

/**
 * @brief Create a new log, meant to be used by the LOG macro
 *
 * @param lvl The level of the log
 * @param src The source of the log
 * @param arg The argument for the log
 * @param file The file of the log
 * @param line The line of the log
 * @param fmt The format of the log
 * @param ... The arguments for the log
 */
void new_log(
    mc_LogLevel_t lvl,
    const char* src,
    void* arg,
    const char* file,
    int line,
    const char* fmt,
    ...
);