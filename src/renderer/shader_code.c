#include <spirv-tools/libspirv.h>
#include <stdlib.h>
#include <string.h>

#include "logger/logger.h"
#include "shader_code.h"

struct ShaderCode {
    size_t size;
    char* bytecode;
};

ShaderCode* shader_code_create(const char* code) {
    CHECK_NULL(code, NULL)
    INFO("assembling shader code");

    ShaderCode* shaderCode = malloc(sizeof(ShaderCode));
    *shaderCode = (ShaderCode){0};

    spv_context ctx = spvContextCreate(SPV_ENV_VULKAN_1_0);
    spv_diagnostic diag;
    spv_binary bin;

    if (spvTextToBinary(ctx, code, strlen(code), &bin, &diag) != SPV_SUCCESS) {
        ERROR("failed to convert text to binary: %s", diag->error);
        spvDiagnosticDestroy(diag);
        spvContextDestroy(ctx);
        shader_code_destroy(shaderCode);
        return NULL;
    }

    shaderCode->size = bin->wordCount * 4;
    shaderCode->bytecode = malloc(shaderCode->size);
    memcpy(shaderCode->bytecode, bin->code, shaderCode->size);

    spvBinaryDestroy(bin);
    spvContextDestroy(ctx);
    return shaderCode;
}

void shader_code_destroy(ShaderCode* shaderCode) {
    CHECK_NULL(shaderCode)
    DEBUG("destroying shader code");
    if (shaderCode->bytecode) free(shaderCode->bytecode);
    free(shaderCode);
}

size_t shader_code_get_size(ShaderCode* shaderCode) {
    CHECK_NULL(shaderCode, 0)
    return shaderCode->size;
}

char* shader_code_get_bytecode(ShaderCode* shaderCode) {
    CHECK_NULL(shaderCode, NULL)
    return shaderCode->bytecode;
}