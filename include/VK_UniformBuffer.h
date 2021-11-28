#ifndef VK_UNIFORMBUFFER_H
#define VK_UNIFORMBUFFER_H
#include <iostream>
#include <utility>
#include <vector>
#include <functional>
#include <vulkan/vulkan.h>
#include "VK_Context.h"

class VK_UniformBuffer : public VK_Deleter
{
public:
    virtual void initBuffer(uint32_t swapImageChainSize) = 0;
    virtual VkWriteDescriptorSet createWriteDescriptorSet(uint32_t index, VkDescriptorSet descriptorSet)const = 0;
    virtual void clearBuffer() = 0;
    virtual void setWriteDataCallback(std::function<uint32_t(char*&, uint32_t)> cb) = 0;
    virtual void update(uint32_t index) = 0;
};

#endif // VK_UNIFORMBUFFER_H
