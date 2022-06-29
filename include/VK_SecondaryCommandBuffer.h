#ifndef VK_SECONDARYCOMMANDBUFFER_H
#define VK_SECONDARYCOMMANDBUFFER_H
#include <vector>
#include <vulkan/vulkan.h>
#include <VK_Deleter.h>

class VK_Context;

class VK_SecondaryCommandBuffer : public VK_Deleter
{
public:
    VK_SecondaryCommandBuffer(VK_Context *vkContext, VkCommandPool pool);
    ~VK_SecondaryCommandBuffer();
public:
    bool create(uint32_t count);
    [[nodiscard]] VkCommandBuffer at(uint32_t index);
    void executeCommandBuffer(VkCommandBuffer command, VkFramebuffer frameBuffer);
    void release() override;
private:
    VK_Context *context = nullptr;
    VkCommandPool commandPool = nullptr;
    std::vector<VkCommandBuffer> buffers;
};

#endif // VK_SECONDARYCOMMANDBUFFER_H
