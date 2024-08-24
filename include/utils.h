#ifndef UTILS_H
#define UTILS_H

#include "example.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define EXPECT(ERROR, FORMAT, ...) {                                                                                                        \
    int macroErrorCode = ERROR;                                                                                                             \
    if(macroErrorCode) {                                                                                                                    \
        fprintf(stderr, "%s -> %s -> %i -> Error(%i):\n\t" FORMAT "\n", __FILE__, __func__, __LINE__, macroErrorCode, ##__VA_ARGS__);  \
        raise(SIGABRT);                                                                                                                     \
    }                                                                                                                                       \
}

uint32_t clamp(uint32_t value, uint32_t min, uint32_t max);

// Reads the '.spv' shader files and creates a shader module
VkShaderModule load_shader(const char* path, App* app);

#endif