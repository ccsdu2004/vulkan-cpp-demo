#ifndef VK_UNIFORMBUFFERIMPL_H
#define VK_UNIFORMBUFFERIMPL_H
#include "VK_UniformBuffer.h"
#include "VK_ContextImpl.h"

class VK_UniformBufferImpl : public VK_UniformBuffer
{
    friend class VK_ContextImpl;
public:
    VK_UniformBufferImpl(VK_ContextImpl* vkContext, VkDevice vkDevice, uint32_t binding, uint32_t bufferSize);
    ~VK_UniformBufferImpl();
public:
    void initBuffer(uint32_t swapImageChainSize)override;

    VkDescriptorBufferInfo createDescriptorBufferInfo(uint32_t index);
    VkWriteDescriptorSet createWriteDescriptorSet(uint32_t index, VkDescriptorSet descriptorSet)const override;

    void clearBuffer()override;
    void release()override;

    void setWriteDataCallback(std::function<uint32_t(char*&, uint32_t)> cb)override;
    void update(uint32_t index)override;
protected:
    VK_ContextImpl* context = nullptr;
    VkDevice device = nullptr;
    uint32_t bindingId = 0;
    uint32_t bufferSize = 0;

    std::function<uint32_t(char*&, uint32_t)> writeDataCallback;

    std::vector<char> bufferData;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<VkDescriptorBufferInfo> bufferInfos;

    bool needClear = false;
};

#endif // VK_UNIFORMBUFFERIMPL_H
