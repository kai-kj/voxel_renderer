#pragma once

#include <stddef.h>

typedef struct ShaderCode ShaderCode;

ShaderCode* shader_code_create(const char* self);
void shader_code_destroy(ShaderCode* self);

size_t shader_code_get_size(ShaderCode* self);
char* shader_code_get_bytecode(ShaderCode* self);