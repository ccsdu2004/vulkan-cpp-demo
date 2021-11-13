#ifndef VK_UNIFORMBUFFERIMPL_H
#define VK_UNIFORMBUFFERIMPL_H
#include "VK_UniformBuffer.h"
#include "VK_Context.h"

class VK_UniformBufferImpl : public VK_UniformBuffer
{
    friend class VK_ContextImpl;
public:
    VK_UniformBufferImpl(VK_Context* vkContext, VkDevice vkDevice, uint32_t bufferSize);
    ~VK_UniformBufferImpl();
public:
    void cleanup()override;
    void initDescriptorSets(VkDescriptorSetLayout descriptorSetLayout, VkDeviceSize swapImageChainSize, VkDescriptorPool pool)override;
    void release()override;

    void setWriteDataCallback(std::function<uint32_t(char*&, uint32_t)> cb)override;
    void bindDescriptorSets(uint32_t index, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)override;
    void update(uint32_t index, float aspect)override;

    void initBuffer(VkDeviceSize swapImageChainSize);
protected:
    VK_Context* context = nullptr;
    VkDevice device = nullptr;
    uint32_t bufferSize = 0;

    std::function<uint32_t(char*&, uint32_t)> writeDataCallback;

    std::vector<char> bufferData;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<VkDescriptorSet> descriptorSets;
};

#endif // VK_UNIFORMBUFFERIMPL_H
