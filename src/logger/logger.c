#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "logger.h"

mc_LogLevel_t logLevel = MC_LOG_LEVEL_INFO;

void set_log_level(mc_LogLevel_t lvl) {
    logLevel = lvl;
}

void new_log(
    mc_LogLevel_t lvl,
    const char* src,
    void* arg,
    const char* file,
    int line,
    const char* fmt,
    ...
) {
    if (lvl < logLevel) return;

    va_list args;
    va_start(args, fmt);
    int message_len = vsnprintf(NULL, 0, fmt, args);
    va_start(args, fmt);
    char* message = malloc(message_len + 1);
    vsnprintf(message, message_len + 1, fmt, args);
    va_end(args);

    for (size_t i = 0; i < strlen(message); i++) {
        if (message[i] == '\n') message[i] = ' ';
    }

    char *lvlStr = "", *colorStr = "";
    switch (lvl) {
        case MC_LOG_LEVEL_DEBUG:
            lvlStr = "DEBUG  ";
            colorStr = "\033[34m";
            break;
        case MC_LOG_LEVEL_INFO:
            lvlStr = "INFO   ";
            colorStr = "\033[32m";
            break;
        case MC_LOG_LEVEL_WARN:
            lvlStr = "WARN   ";
            colorStr = "\033[33m";
            break;
        case MC_LOG_LEVEL_ERROR:
            lvlStr = "ERROR  ";
            colorStr = "\033[31m";
            break;
        default: lvlStr = "UNKNOWN"; break;
    }

    struct tm* tm = localtime(&(time_t){time(NULL)});
    struct timeval tv;
    gettimeofday(&tv, NULL);

    printf(
        "%s%04d-%02d-%02d %02d:%02d:%07.4f │ %s │ %3s │ %s",
        colorStr,
        tm->tm_year + 1900,
        tm->tm_mon + 1,
        tm->tm_mday,
        tm->tm_hour,
        tm->tm_min,
        tm->tm_sec + tv.tv_usec / 1000000.0,
        lvlStr,
        src,
        message
    );

    if (strlen(file) > 0) printf(" \033[2m(%s:%d)\033[0m\n", file, line);
    else printf("\033[0m\n");

    free(message);
}