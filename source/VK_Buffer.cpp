#include "VK_Buffer.h"
#include "VK_Context.h"
#include <iostream>

VK_Buffer::VK_Buffer(VK_Context* vkContext, VkDevice vkDevice):
    context(vkContext),
    device(vkDevice)
{
}

VK_Buffer::~VK_Buffer()
{
}

VK_Context *VK_Buffer::getContext() const
{
    return context;
}

void VK_Buffer::release()
{
    if(device) {
        if(buffer)
            vkDestroyBuffer(device, buffer, context->getAllocation());
        if(bufferMemory)
            vkFreeMemory(device, bufferMemory, context->getAllocation());
    }

    context->removeBuffer(this);
    delete this;
}
