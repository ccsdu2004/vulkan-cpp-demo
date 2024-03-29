#ifndef VK_BUFFER_H
#define VK_BUFFER_H
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "VK_Deleter.h"
#include "VK_Vertex.h"

class VK_Context;

class VK_Buffer : public VK_Deleter
{
public:
    VK_Buffer(VK_Context *context);
    virtual ~VK_Buffer();
public:
    virtual size_t getDataCount() const = 0;
    [[nodiscard]] VK_Context *getContext()const;
    virtual void release() override;
public:
    virtual void render(VkCommandBuffer command) = 0;
protected:
    VK_Context *context = nullptr;
    VkBuffer buffer = 0;
    VkDeviceMemory bufferMemory = 0;
};

#endif // VK_BUFFER_H
