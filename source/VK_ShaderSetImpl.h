#ifndef VK_SHADERSETIMPL_H
#define VK_SHADERSETIMPL_H
#include <vector>
#include <map>
#include "VK_ShaderSet.h"

class VK_ShaderSetImpl : public VK_ShaderSet
{
public:
    VK_ShaderSetImpl() = delete;
    VK_ShaderSetImpl(VkDevice device);
    ~VK_ShaderSetImpl();
public:
    void release()override;

    void appendAttributeDescription(int index, int size)override;
    VkVertexInputBindingDescription* getBindingDescription()override;
    size_t getAttributeDescriptionCount()const override;
    const VkVertexInputAttributeDescription* getAttributeDescriptionData()const override;

    void addDescriptorSetLayoutBinding(const VkDescriptorSetLayoutBinding& binding)override;
    size_t getDescriptorSetLayoutBindingCount()const override;
    const VkDescriptorSetLayoutBinding* getDescriptorSetLayoutBindingData()const override;

    size_t getDescriptorPoolSizeCount()const override;
    const VkDescriptorPoolSize *getDescriptorPoolSizeData()const override;
    void updateDescriptorPoolSize(int32_t size)override;

    bool addShader(const std::string &spvFile, VkShaderStageFlagBits type,
                   const char* entryPoint = "main")override;
    bool isValid()override;
    VkPipelineShaderStageCreateInfo *getCreateInfoData()override;
    size_t getCreateInfoCount()override;
private:
    VkShaderModule createShaderModule(const std::string &spvFile);
private:
    VkDevice vkDevice = nullptr;
    std::vector<int> sizeTable;
    VkVertexInputBindingDescription bindingDescription{};
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
    std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
};

#endif // VK_SHADERSETIMPL_H
