#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "logger.h"

log_fn currLogFunction = basic_log_fn;
void* currLogFunctionArg = NULL;

void set_log_fn(log_fn fn, void* arg) {
    currLogFunction = fn;
    currLogFunctionArg = arg;
}

void basic_log_fn(
    int lvl,
    char* src,
    char* file,
    int line,
    char* msg,
    void* arg
) {
    for (char* c = msg; *c != '\0'; c++) {
        if (*c == '\n') *c = ' ';
    }
    printf("%s\n", msg);
}

void new_log(
    mc_LogLevel_t lvl,
    char* src,
    void* arg,
    char* file,
    int line,
    char* fmt,
    ...
) {
    va_list args;
    va_start(args, fmt);
    int message_len = vsnprintf(NULL, 0, fmt, args);
    va_start(args, fmt);
    char* message = malloc(message_len + 1);
    vsnprintf(message, message_len + 1, fmt, args);
    va_end(args);

    currLogFunction(lvl, src, file, line, message, currLogFunctionArg);

    free(message);
}