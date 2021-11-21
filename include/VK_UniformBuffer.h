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
    virtual void initBuffer(VkDeviceSize swapImageChainSizel) = 0;
    virtual void initDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout, VkDeviceSize swapImageChainSize, VkDescriptorPool pool) = 0;
    virtual void clearBuffer() = 0;
    virtual void setWriteDataCallback(std::function<uint32_t(char*&, uint32_t)> cb) = 0;
    virtual void bindDescriptorSets(uint32_t index, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) = 0;
    virtual void update(uint32_t index) = 0;
};

#endif // VK_UNIFORMBUFFER_H
