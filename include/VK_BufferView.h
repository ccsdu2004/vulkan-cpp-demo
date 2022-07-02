#pragma once
#include <vulkan/vulkan.h>

class VK_BufferView
{
public:
    [[nodiscard]] virtual VkBufferView *getBufferView() = 0;
};
