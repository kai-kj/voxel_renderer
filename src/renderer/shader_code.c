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
    if (!code) return NULL;
    INFO("assembling shader code");

    ShaderCode* self = malloc(sizeof(ShaderCode));
    *self = (ShaderCode){0};

    spv_context ctx = spvContextCreate(SPV_ENV_VULKAN_1_0);
    spv_diagnostic diag;
    spv_binary bin;

    if (spvTextToBinary(ctx, code, strlen(code), &bin, &diag) != SPV_SUCCESS) {
        ERROR("failed to convert text to binary: %s", diag->error);
        spvDiagnosticDestroy(diag);
        spvContextDestroy(ctx);
        shader_code_destroy(self);
        return NULL;
    }

    self->size = bin->wordCount * 4;
    self->bytecode = malloc(self->size);
    memcpy(self->bytecode, bin->code, self->size);

    spvBinaryDestroy(bin);
    spvContextDestroy(ctx);
    return self;
}

void shader_code_destroy(ShaderCode* self) {
    if (!self) return;
    INFO("destroying shader code");
    if (self->bytecode) free(self->bytecode);
    free(self);
}

size_t shader_code_get_size(ShaderCode* self) {
    if (!self) return 0;
    return self->size;
}

char* shader_code_get_bytecode(ShaderCode* self) {
    if (!self) return NULL;
    return self->bytecode;
}