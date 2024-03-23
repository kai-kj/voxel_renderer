#ifndef vec_H_INCLUDE_GUARD
#define vec_H_INCLUDE_GUARD

typedef unsigned int uint;

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
        int x;
        int y;
    };
    struct {
        int r;
        int g;
    };
    struct {
        int _0;
        int _1;
    };
} ivec2;

typedef union {
    struct {
        int x;
        int y;
        int z;
    };
    struct {
        int r;
        int g;
        int b;
    };
    struct {
        int _0;
        int _1;
        int _2;
        int _padding;
    };
} ivec3;

typedef union {
    struct {
        int x;
        int y;
        int z;
        int w;
    };
    struct {
        int r;
        int g;
        int b;
        int a;
    };
    struct {
        int _0;
        int _1;
        int _2;
        int _3;
    };
} ivec4;

typedef union {
    struct {
        uint x;
        uint y;
    };
    struct {
        uint r;
        uint g;
    };
    struct {
        uint _0;
        uint _1;
    };
} uvec2;

typedef union {
    struct {
        uint x;
        uint y;
        uint z;
    };
    struct {
        uint r;
        uint g;
        uint b;
    };
    struct {
        uint _0;
        uint _1;
        uint _2;
        uint _padding;
    };
} uvec3;

typedef union {
    struct {
        uint x;
        uint y;
        uint z;
        uint w;
    };
    struct {
        uint r;
        uint g;
        uint b;
        uint a;
    };
    struct {
        uint _0;
        uint _1;
        uint _2;
        uint _3;
    };
} uvec4;

#ifdef vec_SHORT_NAMES
#define uint uint
#define vec2 vec2
#define vec3 vec3
#define vec4 vec4
#define ivec2 ivec2
#define ivec3 ivec3
#define ivec4 ivec4
#define uvec2 uvec2
#define uvec3 uvec3
#define uvec4 uvec4
#endif

#endif
