#pragma once
#include <memory>
#include <vulkan/vulkan.h>
#include <VK_Context.h>

class VK_SecondaryCommandBufferCallback
{
public:
    virtual void execute(VK_Context *context, VkCommandBuffer commandBuffer, uint32_t current,
                         uint32_t total) = 0;
};

