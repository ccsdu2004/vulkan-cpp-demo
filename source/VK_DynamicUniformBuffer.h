#pragma once
#include <VK_UniformBuffer.h>
#include "VK_UniformBuffer.h"
#include "VK_ContextImpl.h"
#include "VK_BufferViewImpl.h"

class VK_DynamicUniformBuffer : public VK_UniformBuffer
{
    friend class VK_ContextImpl;
public:
    VK_DynamicUniformBuffer(VK_ContextImpl *vkContext, uint32_t binding, uint32_t bufferSize,
                            uint32_t count);
    ~VK_DynamicUniformBuffer();
public:
    bool isDynamicBuffer()const override;
    uint32_t getBufferSize()const override;
    uint32_t getBufferCount()const override;
    void initBuffer(uint32_t swapImageChainSize)override;

    VkWriteDescriptorSet createWriteDescriptorSet(uint32_t index,
                                                  VkDescriptorSet descriptorSet)const override;

    void clearBuffer()override;
    void release()override;

    void setWriteDataCallback(std::function<uint32_t(char *&, uint32_t)> cb)override;
    void update(uint32_t index)override;
protected:
    VK_ContextImpl *context = nullptr;
    uint32_t bindingId = 0;
    uint32_t bufferSize = 0;
    uint32_t uniformCount = 0;

    std::function<uint32_t(char *&, uint32_t)> writeDataCallback;

    std::vector<char> bufferData;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<VkDescriptorBufferInfo> bufferInfos;

    bool needClear = false;
};
