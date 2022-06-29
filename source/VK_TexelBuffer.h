#ifndef VK_TEXELBUFFER_H
#define VK_TEXELBUFFER_H
#include <VK_UniformBuffer.h>
#include <VK_ContextImpl.h>
#include <VK_BufferViewImpl.h>

class VK_TexelBuffer : public VK_UniformBuffer
{
    friend class VK_ContextImpl;
public:
    VK_TexelBuffer() = delete;
    VK_TexelBuffer(VK_ContextImpl *vkContext, uint32_t binding, uint32_t bufferSize);
    ~VK_TexelBuffer();
public:
    void initBuffer(uint32_t swapImageChainSize)override;

    [[nodiscard]] VkWriteDescriptorSet createWriteDescriptorSet(uint32_t index,
                                                                VkDescriptorSet descriptorSet)const override;

    void clearBuffer()override;
    void release()override;

    void setWriteDataCallback(std::function<uint32_t(char *&, uint32_t)> cb)override;
    void update(uint32_t index)override;
protected:
    VK_ContextImpl *context = nullptr;
    uint32_t bindingId = 0;
    uint32_t bufferSize = 0;

    std::function<uint32_t(char *&, uint32_t)> writeDataCallback;

    std::vector<char> bufferData;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<VkDescriptorBufferInfo> bufferInfos;

    std::vector<std::shared_ptr<VK_BufferView>> bufferViews;

    bool needClear = false;
};

#endif // VK_TEXELBUFFER_H
