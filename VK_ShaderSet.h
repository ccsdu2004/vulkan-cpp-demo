#ifndef VK_SHADERSET_H
#define VK_SHADERSET_H
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "VK_Deleter.h"

class VK_ShaderSet : public VK_Deleter
{
public:
    virtual bool addShader(const std::string &spvFile, VkShaderStageFlagBits type,
                           const std::string &entryPoint = std::string("main")) = 0;
    virtual bool isValid() = 0;
    virtual VkPipelineShaderStageCreateInfo* getCreateInfoData() = 0;
    virtual int getCreateInfoCount() = 0;
};

#endif // VK_SHADERSET_H
