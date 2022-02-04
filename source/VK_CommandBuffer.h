#ifndef VK_COMMANDBUFFER_H
#define VK_COMMANDBUFFER_H
#include <vulkan/vulkan.h>

class VK_Context;

class VK_CommandBuffer
{
public:
    VK_CommandBuffer(VK_Context* vkContext);
private:
    VK_Context* context = nullptr;
};

#endif // VK_COMMANDBUFFER_H
