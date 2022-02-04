#include <iostream>
#include <VK_Context.h>
#include <VK_CommandPool.h>
#include <VK_SecondaryCommandBuffer.h>

VK_CommandPool::VK_CommandPool(VK_Context* vkContext, const QueueFamilyIndices &index):
    context(vkContext)
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = index.graphicsFamily.value();
    poolInfo.pNext = nullptr;
    poolInfo.flags = 0;

    if (vkCreateCommandPool(context->getDevice(), &poolInfo, context->getAllocation(), &pool) != VK_SUCCESS) {
        std::cerr << "failed to create command pool!" << std::endl;
    }
}

void VK_CommandPool::release()
{
    vkDestroyCommandPool(context->getDevice(), pool, context->getAllocation());
    delete this;
}

VkCommandPool VK_CommandPool::getCommandPool()const
{
    return pool;
}

VK_SecondaryCommandBuffer *VK_CommandPool::createSecondaryCommand(uint32_t count)
{
    auto commandBuffer = new VK_SecondaryCommandBuffer(context, pool);
    commandBuffer->create(count);
    return commandBuffer;
}

VkCommandBuffer VK_CommandPool::beginSingleTimeCommands(uint32_t flag)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = pool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(context->getDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flag;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VK_CommandPool::endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue queue)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(context->getDevice(), pool, 1, &commandBuffer);
}

