#include <shaderc/shaderc.h>
#include <stdlib.h>
#include <string.h>

#include "logger/logger.h"
#include "shader_compiler.h"

char* int_to_string(int i) {
    int len = snprintf(NULL, 0, "%d", i);
    char* str = malloc(len + 1);
    snprintf(str, len + 1, "%d", i);
    return str;
}

SPIRVCode compile_glsl(
    const char* name,
    const char* code,
    const char* entrypoint,
    uvec2 wgSize
) {
    CHECK_NULL(code, (SPIRVCode){0, NULL});
    CHECK_NULL(entrypoint, (SPIRVCode){0, NULL});
    INFO("compiling shader \"%s\", entrypoint: \"%s\"", name, entrypoint);

    shaderc_compile_options_t options = shaderc_compile_options_initialize();
    if (!options) {
        ERROR("failed to initialize shader compiler options");
        return (SPIRVCode){0, NULL};
    }

    char* wgSizeX = int_to_string((int)wgSize.x);
    shaderc_compile_options_add_macro_definition(
        options,
        "WORKGROUP_SIZE_X",
        strlen("WORKGROUP_SIZE_X"),
        wgSizeX,
        strlen(wgSizeX)
    );
    free(wgSizeX);

    char* wgSizeY = int_to_string((int)wgSize.y);
    shaderc_compile_options_add_macro_definition(
        options,
        "WORKGROUP_SIZE_Y",
        strlen("WORKGROUP_SIZE_Y"),
        wgSizeY,
        strlen(wgSizeY)
    );
    free(wgSizeY);

    shaderc_compile_options_set_optimization_level(
        options,
        shaderc_optimization_level_performance
    );

    shaderc_compiler_t compiler = shaderc_compiler_initialize();
    if (!compiler) {
        ERROR("failed to initialize shader compiler");
        return (SPIRVCode){0, NULL};
    }

    shaderc_compilation_result_t result = shaderc_compile_into_spv(
        compiler,
        code,
        strlen(code),
        shaderc_glsl_compute_shader,
        name,
        entrypoint,
        options
    );

    shaderc_compile_options_release(options);

    if (shaderc_result_get_num_errors(result)
        || shaderc_result_get_num_warnings(result)) {
        ERROR("failed to compile shader code");
        ERROR(shaderc_result_get_error_message(result));
        shaderc_result_release(result);
        shaderc_compiler_release(compiler);
        return (SPIRVCode){0, NULL};
    }

    size_t size = shaderc_result_get_length(result);
    char* spirv = malloc(size);
    memcpy(spirv, shaderc_result_get_bytes(result), size);

    shaderc_result_release(result);
    shaderc_compiler_release(compiler);

    return (SPIRVCode){size, spirv};
}