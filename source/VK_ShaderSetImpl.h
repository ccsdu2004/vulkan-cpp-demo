#ifndef VK_SHADERSETIMPL_H
#define VK_SHADERSETIMPL_H
#include <vector>
#include <map>
#include <list>
#include "VK_ShaderSet.h"

class VK_ImageView;
class VK_DescriptorSet;
class VK_UniformBuffer;
class VK_ContextImpl;

class VK_ShaderSetImpl : public VK_ShaderSet
{
public:
    VK_ShaderSetImpl() = delete;
    VK_ShaderSetImpl(VK_ContextImpl *vkContext);
    ~VK_ShaderSetImpl();
public:
    void release()override;

    void appendVertexAttributeDescription(uint32_t index, uint32_t size, VkFormat format,
                                          uint32_t offset, uint32_t binding = 0) override;
    void appendVertexInputBindingDescription(uint32_t stride, uint32_t binding = 0,
                                             VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX)override;

    size_t getVertexAttributeDescriptionCount()const override;
    const VkVertexInputAttributeDescription *getVertexAttributeDescriptionData()const override;

    size_t getVertexInputBindingDescriptionCount()const override;
    virtual const VkVertexInputBindingDescription *getVertexInputBindingDescriptionData()const override;

    void addDescriptorSetLayoutBinding(const VkDescriptorSetLayoutBinding &binding)override;
    size_t getDescriptorSetLayoutBindingCount()const override;
    const VkDescriptorSetLayoutBinding *getDescriptorSetLayoutBindingData()const override;

    size_t getDescriptorPoolSizeCount()const override;
    const VkDescriptorPoolSize *getDescriptorPoolSizeData()const override;
    void updateDescriptorPoolSize(int32_t size)override;

    bool addShader(const std::string &spvFile, VkShaderStageFlagBits type,
                   const char *entryPoint = "main")override;
public:
    VK_UniformBuffer *addUniformBuffer(uint32_t binding, uint32_t bufferSize)override;
    VK_UniformBuffer *addTexelBuffer(uint32_t binding, uint32_t bufferSize) override;

    void initUniformBuffer() override;
    void clearUniformBuffer()override;

    void addImageView(VK_ImageView *imageView, uint32_t binding = 1)override;

    bool isValid()override;
    VkPipelineShaderStageCreateInfo *getCreateInfoData()override;
    size_t getCreateInfoCount()override;

    void updateDescriptorSet(std::shared_ptr<VK_DescriptorSets> descriptorSet)override;
    void update(uint32_t index)override;
private:
    VkShaderModule createShaderModule(const std::string &spvFile);
private:
    VK_ContextImpl *context = nullptr;

    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;

    std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    std::vector<VkDescriptorPoolSize> descriptorPoolSizes;

    std::list<VK_UniformBuffer *> uniformBuffers;
    std::map<VK_ImageView *, uint32_t> imageViews;
};

#endif // VK_SHADERSETIMPL_H
