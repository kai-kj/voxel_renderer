#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"

log_fn currLogFunction = basic_log_fn;
void* currLogFunctionArg = NULL;

void set_log_fn(log_fn fn, void* arg) {
    currLogFunction = fn;
    currLogFunctionArg = arg;
}

void basic_log_fn(
    void* arg,
    int lvl,
    const char* src,
    const char* file,
    int line,
    const char* msg
) {
    for (char* c = (char*)msg; *c != '\0'; c++) {
        if (*c == '\n') *c = ' ';
    }
    printf("%s\n", msg);
}

void new_log(
    void* arg,
    mc_LogLevel lvl,
    const char* src,
    const char* file,
    int line,
    const char* fmt,
    ...
) {
    va_list args;
    va_start(args, fmt);
    int message_len = vsnprintf(NULL, 0, fmt, args);
    va_start(args, fmt);
    char* message = malloc(message_len + 1);
    vsnprintf(message, message_len + 1, fmt, args);
    va_end(args);

    currLogFunction(currLogFunctionArg, lvl, src, file, line, message);

    free(message);
}