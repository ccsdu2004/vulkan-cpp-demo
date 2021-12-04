#ifndef VK_SHADERSET_H
#define VK_SHADERSET_H
#include <vector>
#include <string>
#include <vulkan/vulkan.h>
#include "VK_Deleter.h"

class VK_ShaderSet : public VK_Deleter
{
public:
    virtual void appendAttributeDescription(int index, int size) = 0;
    virtual VkVertexInputBindingDescription* getBindingDescription() = 0;
    virtual size_t getAttributeDescriptionCount()const = 0;
    virtual const VkVertexInputAttributeDescription* getAttributeDescriptionData()const = 0;

    static VkDescriptorSetLayoutBinding createDescriptorSetLayoutBinding(uint32_t id, VkDescriptorType type, VkShaderStageFlagBits flag);
    virtual void addDescriptorSetLayoutBinding(const VkDescriptorSetLayoutBinding& binding) = 0;
    virtual size_t getDescriptorSetLayoutBindingCount()const = 0;
    virtual const VkDescriptorSetLayoutBinding* getDescriptorSetLayoutBindingData()const = 0;

    virtual size_t getDescriptorPoolSizeCount()const = 0;
    virtual const VkDescriptorPoolSize *getDescriptorPoolSizeData()const = 0;
    virtual void updateDescriptorPoolSize(int32_t size) = 0;

    virtual bool addShader(const std::string &spvFile, VkShaderStageFlagBits type,
                           const char* entryPoint = "main") = 0;
    virtual bool isValid() = 0;
    virtual VkPipelineShaderStageCreateInfo* getCreateInfoData() = 0;
    virtual size_t getCreateInfoCount() = 0;
};

#endif // VK_SHADERSET_H
