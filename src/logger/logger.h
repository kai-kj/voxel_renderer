#pragma once

#include <stdio.h>

#include "microcompute.h"

typedef void (*log_fn)(void*, int, const char*, const char*, int, const char*);

/**
 * @brief Log a message
 * @param lvl The level of the message
 * @param fmt The format of the message
 * @param ... The arguments for the message
 */
#define LOG(lvl, fmt, ...)                                                     \
    new_log(                                                                   \
        NULL,                                                                  \
        lvl,                                                                   \
        "app",                                                                 \
        __FILE__,                                                              \
        __LINE__,                                                              \
        fmt __VA_OPT__(, ) __VA_ARGS__                                         \
    )

/**
 * @brief Log a message at the debug level
 * @param fmt The format of the message
 * @param ... The arguments for the message
 */
#define DEBUG(fmt, ...) LOG(MC_LOG_LEVEL_DEBUG, fmt __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief Log a message at the info level
 * @param fmt The format of the message
 * @param ... The arguments for the message
 */
#define INFO(fmt, ...) LOG(MC_LOG_LEVEL_INFO, fmt __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief Log a message at the warn level
 * @param fmt The format of the message
 * @param ... The arguments for the message
 */
#define WARN(fmt, ...) LOG(MC_LOG_LEVEL_WARN, fmt __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief Log a message at the error level
 * @param fmt The format of the message
 * @param ... The arguments for the message
 */
#define ERROR(fmt, ...) LOG(MC_LOG_LEVEL_ERROR, fmt __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief Check if a pointer is NULL and log an error if it is
 * @param ptr The pointer to check
 * @param ret The value to return if the pointer is NULL
 */
#define CHECK_NULL(ptr, ...)                                                   \
    if (!ptr) {                                                                \
        WARN("%s: %s is null", __func__, #ptr);                                \
        return __VA_ARGS__;                                                    \
    }

/**
 * @brief Set the log function
 * @param fn The log function
 * @param arg The argument for the log function
 */
void set_log_fn(log_fn fn, void* arg);

/**
 * @brief A basic log function that logs to stdout
 */
void basic_log_fn(
    void* arg,
    int lvl,
    const char* src,
    const char* file,
    int line,
    const char* msg
);

/**
 * @brief Create a new log, meant to be used by the LOG macro
 * @param arg The argument for the log function
 * @param lvl The level of the log
 * @param src The source of the log
 * @param file The file of the log
 * @param line The line of the log
 * @param fmt The format of the log
 * @param ... The arguments for the log
 */
void new_log(
    void* arg,
    mc_LogLevel lvl,
    const char* src,
    const char* file,
    int line,
    const char* fmt,
    ...
);
