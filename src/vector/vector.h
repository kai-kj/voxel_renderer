#pragma once

#include <stdint.h>

typedef union {
    struct {
        float x;
        float y;
    };
    struct {
        float r;
        float g;
    };
    struct {
        float _0;
        float _1;
    };
} vec2;

typedef union {
    struct {
        float x;
        float y;
        float z;
    };
    struct {
        float r;
        float g;
        float b;
    };
    struct {
        float _0;
        float _1;
        float _2;
        float _padding;
    };
} vec3;

typedef union {
    struct {
        float x;
        float y;
        float z;
        float w;
    };
    struct {
        float r;
        float g;
        float b;
        float a;
    };
    struct {
        float _0;
        float _1;
        float _2;
        float _3;
    };
} vec4;

typedef union {
    struct {
        int32_t x;
        int32_t y;
    };
    struct {
        int32_t r;
        int32_t g;
    };
    struct {
        int32_t _0;
        int32_t _1;
    };
} ivec2;

typedef union {
    struct {
        int32_t x;
        int32_t y;
        int32_t z;
    };
    struct {
        int32_t r;
        int32_t g;
        int32_t b;
    };
    struct {
        int32_t _0;
        int32_t _1;
        int32_t _2;
        int32_t _padding;
    };
} ivec3;

typedef union {
    struct {
        int32_t x;
        int32_t y;
        int32_t z;
        int32_t w;
    };
    struct {
        int32_t r;
        int32_t g;
        int32_t b;
        int32_t a;
    };
    struct {
        int32_t _0;
        int32_t _1;
        int32_t _2;
        int32_t _3;
    };
} ivec4;

typedef union {
    struct {
        uint32_t x;
        uint32_t y;
    };
    struct {
        uint32_t r;
        uint32_t g;
    };
    struct {
        uint32_t _0;
        uint32_t _1;
    };
} uvec2;

typedef union {
    struct {
        uint32_t x;
        uint32_t y;
        uint32_t z;
    };
    struct {
        uint32_t r;
        uint32_t g;
        uint32_t b;
    };
    struct {
        uint32_t _0;
        uint32_t _1;
        uint32_t _2;
        uint32_t _padding;
    };
} uvec3;

typedef union {
    struct {
        uint32_t x;
        uint32_t y;
        uint32_t z;
        uint32_t w;
    };
    struct {
        uint32_t r;
        uint32_t g;
        uint32_t b;
        uint32_t a;
    };
    struct {
        uint32_t _0;
        uint32_t _1;
        uint32_t _2;
        uint32_t _3;
    };
} uvec4;