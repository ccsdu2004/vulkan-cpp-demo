#ifndef VK_UNIFORMBUFFER_H
#define VK_UNIFORMBUFFER_H
#include <iostream>
#include <utility>
#include <vector>
#include <functional>
#include "VK_Context.h"
#include "VK_Buffer.h"

class VK_UniformBuffer : public VK_Deleter
{
    using BufferNode = std::pair<VkBuffer, VkDeviceMemory>;
public:
    VK_UniformBuffer() = delete;
    VK_UniformBuffer(VK_Context* context, VkDevice vkDevice);
    virtual ~VK_UniformBuffer();
public:
    void release()override;

    bool createBuffer(VkDeviceSize bufferSize, VkDeviceSize swapImageChainSize);
    void setWriteDataCallback(std::function<void(char*&, VkDeviceSize)> cb);
    VkDeviceSize getBufferSize()const;
    void update(VkDeviceSize index);
protected:
    VK_Context* context = nullptr;
    VkDevice device = nullptr;
    std::vector<BufferNode> uniformBuffers;
    std::function<void(char*&, VkDeviceSize)> writeDataCallback;
    std::vector<char> bufferData;
};

#endif // VK_UNIFORMBUFFER_H
