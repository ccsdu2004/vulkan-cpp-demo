#include "VK_Buffer.h"
#include "VK_Context.h"
#include <iostream>

VK_Buffer::VK_Buffer(VK_Context* vkContext):
    context(vkContext)
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
    if(buffer)
        vkDestroyBuffer(context->getDevice(), buffer, context->getAllocation());
    if(bufferMemory)
        vkFreeMemory(context->getDevice(), bufferMemory, context->getAllocation());

    context->removeBuffer(this);
    delete this;
}
