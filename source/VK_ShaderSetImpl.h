#ifndef VK_SHADERSETIMPL_H
#define VK_SHADERSETIMPL_H
#include <vector>
#include "../VK_ShaderSet.h"

class VK_ShaderSetImpl : public VK_ShaderSet
{
public:
    VK_ShaderSetImpl() = delete;
    VK_ShaderSetImpl(VkDevice device);
    ~VK_ShaderSetImpl();
public:
    void release()override;

    bool addShader(const std::string &spvFile, VkShaderStageFlagBits type,
                   const std::string &entryPoint = std::string("main"))override;
    bool isValid()override;
    VkPipelineShaderStageCreateInfo *getCreateInfoData()override;
    int getCreateInfoCount()override;
private:
    VkShaderModule createShaderModule(const std::string &spvFile);
private:
    VkDevice vkDevice = nullptr;
    std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
};

#endif // VK_SHADERSETIMPL_H
