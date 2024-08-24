#include "../include/utils.h"

uint32_t clamp(uint32_t value, uint32_t min, uint32_t max) {
    if(value < min) {
        return min;
    } else if(value > max) {
        return max;
    }
    return value;
}

VkShaderModule load_shader(const char* path, App* app){
    int fd = open(path, O_RDONLY, 0666);
    EXPECT(fd<0, "Couldn't open shader file: %s", path)

    struct stat st;
    stat(path, &st);
    size_t size = st.st_size;
    size_t curr = 0;
    int i = 0;
    uint32_t* code = (uint32_t*) malloc(size);    
    char buffer[11];
    while(curr<size){
        if(read(fd, buffer, 11)<=0)
            break;
        curr += 11;
        buffer[10] = '\0';
        code[i++] = (uint32_t)strtol(buffer, NULL, 16);

        if(read(fd, buffer, 11)<=0)
            break;
        curr += 11;
        buffer[10] = '\0';
        if(strlen(buffer)!=10)
            break;
        code[i++] = (uint32_t)strtol(buffer, NULL, 16);
        
        if(read(fd, buffer, 11)<=0)
            break;
        curr += 11;
        buffer[10] = '\0';
        code[i++] = (uint32_t)strtol(buffer, NULL, 16);
        
        if(read(fd, buffer, 11)<=0)
            break;
        curr += 11;
        buffer[10] = '\0';
        code[i++] = (uint32_t)strtol(buffer, NULL, 16);

        if(read(fd, buffer, 1)<=0)
            break;
        curr++;
    }

    VkShaderModule shaderModule;
    EXPECT(vkCreateShaderModule(app->device, &(VkShaderModuleCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pCode = code,
        .codeSize = i*sizeof(uint32_t),
    }, NULL, &shaderModule), "Couldn't create vertex shader module")
    free(code);
    close(fd);
    return shaderModule;
}
