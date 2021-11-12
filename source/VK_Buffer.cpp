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

void VK_Buffer::release()
{
    if(device) {
        if(buffer)
            vkDestroyBuffer(device, buffer, nullptr);
        if(bufferMemory)
            vkFreeMemory(device, bufferMemory, nullptr);
    }

    context->removeBuffer(this);
    delete this;
}
