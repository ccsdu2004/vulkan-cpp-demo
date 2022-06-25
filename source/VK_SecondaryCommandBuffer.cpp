#include <VK_SecondaryCommandBuffer.h>
#include <VK_Context.h>
#include <iostream>

VK_SecondaryCommandBuffer::VK_SecondaryCommandBuffer(VK_Context *vkContext, VkCommandPool pool):
    context(vkContext),
    commandPool(pool)
{
}

VK_SecondaryCommandBuffer::~VK_SecondaryCommandBuffer()
{
}

bool VK_SecondaryCommandBuffer::create(uint32_t count)
{
    VkCommandBufferAllocateInfo cmdAlloc = {};
    cmdAlloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdAlloc.pNext = NULL;
    cmdAlloc.commandPool = commandPool;
    cmdAlloc.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    cmdAlloc.commandBufferCount = count;

    buffers.resize(count);
    return vkAllocateCommandBuffers(context->getDevice(), &cmdAlloc, buffers.data()) == VK_SUCCESS;
}

VkCommandBuffer VK_SecondaryCommandBuffer::at(uint32_t index)
{
    return buffers.at(index);
}

void VK_SecondaryCommandBuffer::executeCommandBuffer(VkCommandBuffer command,
        VkFramebuffer frameBuffer)
{
    VkRenderPassBeginInfo rpBegin;
    rpBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBegin.pNext = NULL;
    rpBegin.renderPass = context->getRenderPass();
    rpBegin.framebuffer = frameBuffer;
    rpBegin.renderArea.offset.x = 0;
    rpBegin.renderArea.offset.y = 0;
    rpBegin.renderArea.extent.width = context->getSwapChainExtent().width;
    rpBegin.renderArea.extent.height = context->getSwapChainExtent().height;
    rpBegin.clearValueCount = 2;

    VkClearValue cv[2] = {};
    cv[1].depthStencil = {1.0f, 0};
    rpBegin.pClearValues = cv;

    vkCmdBeginRenderPass(command, &rpBegin, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
    vkCmdExecuteCommands(command, buffers.size(), buffers.data());
    vkCmdEndRenderPass(command);
}

void VK_SecondaryCommandBuffer::release()
{
    vkFreeCommandBuffers(context->getDevice(), commandPool, buffers.size(), buffers.data());
    buffers.clear();
    delete this;
}
