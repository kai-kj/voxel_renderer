#ifndef COMPILER_H
#define COMPILER_H

#include "vector.h"

typedef struct SPIRVCode {
    size_t size;
    char* code;
} SPIRVCode;

SPIRVCode compile_glsl(
    const char* name,
    const char* code,
    const char* entrypoint,
    uvec2 wgSize
);

#endif // COMPILER_H
