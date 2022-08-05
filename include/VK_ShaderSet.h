#ifndef VK_SHADERSET_H
#define VK_SHADERSET_H
#include <vector>
#include <string>
#include <list>
#include <memory>
#include <vulkan/vulkan.h>
#include "VK_Deleter.h"

class VK_DescriptorSets;
class VK_ImageView;
class VK_UniformBuffer;
class VK_DynamicUniformBuffer;

class VK_ShaderSet : public VK_Deleter
{
public:
    virtual void appendVertexAttributeDescription(uint32_t index, uint32_t size, VkFormat format,
                                                  uint32_t offset, uint32_t binding = 0) = 0;
    virtual void appendVertexInputBindingDescription(uint32_t stride, uint32_t binding = 0,
                                                     VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX) = 0;

    virtual size_t getVertexAttributeDescriptionCount()const = 0;
    [[nodiscard]] virtual const VkVertexInputAttributeDescription *getVertexAttributeDescriptionData()
    const = 0;

    virtual size_t getVertexInputBindingDescriptionCount()const = 0;
    [[nodiscard]] virtual const VkVertexInputBindingDescription *getVertexInputBindingDescriptionData()
    const = 0;

    static VkDescriptorSetLayoutBinding createDescriptorSetLayoutBinding(uint32_t id,
                                                                         VkDescriptorType type, VkShaderStageFlagBits flag);

    virtual void addDescriptorSetLayoutBinding(const VkDescriptorSetLayoutBinding &binding) = 0;
    virtual size_t getDescriptorSetLayoutBindingCount()const = 0;
    [[nodiscard]] virtual const VkDescriptorSetLayoutBinding *getDescriptorSetLayoutBindingData()const =
        0;

    virtual size_t getDescriptorPoolSizeCount()const = 0;
    [[nodiscard]] virtual const VkDescriptorPoolSize *getDescriptorPoolSizeData()const = 0;
    virtual void updateDescriptorPoolSize(int32_t size) = 0;

    virtual bool addShader(const std::string &spvFile, VkShaderStageFlagBits type,
                           const char *entryPoint = "main") = 0;
public:
    [[nodiscard]] virtual VK_UniformBuffer *addUniformBuffer(uint32_t binding, uint32_t bufferSize) = 0;
    [[nodiscard]] virtual VK_UniformBuffer *addDynamicUniformBuffer(uint32_t binding,
                                                                    uint32_t bufferSize, uint32_t count) = 0;
    [[nodiscard]] virtual VK_UniformBuffer *addTexelBuffer(uint32_t binding, uint32_t bufferSize) = 0;
    [[nodiscard]] virtual VK_UniformBuffer *addStorageBuffer(uint32_t binding, uint32_t bufferSize) = 0;
    [[nodiscard]] virtual VK_UniformBuffer *addStorageTexelBuffer(uint32_t biding,
                                                                  uint32_t bufferSize) = 0;
    virtual void initUniformBuffer() = 0;
    virtual void clearUniformBuffer() = 0;

    virtual void addImageView(VK_ImageView *imageView, uint32_t binding = 1) = 0;
public:
    virtual bool isValid() = 0;
    [[nodiscard]] virtual VkPipelineShaderStageCreateInfo *getCreateInfoData() = 0;
    virtual size_t getCreateInfoCount() = 0;

    virtual void updateDescriptorSet(std::shared_ptr<VK_DescriptorSets> descriptorSet) = 0;
    virtual void update(uint32_t index) = 0;
};

#endif // VK_SHADERSET_H
