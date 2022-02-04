#ifndef VK_RENDERPASS_H
#define VK_RENDERPASS_H
#include <vulkan/vulkan.h>
#include <VK_Deleter.h>

class VK_ContextImpl;

class VK_RenderPass : public VK_Deleter
{
public:
    VK_RenderPass() = delete;
    VK_RenderPass(VK_ContextImpl *vkContext);
public:
    void create(VkFormat format, VkSampleCountFlagBits flag);
    VkRenderPass getRenderPass()const
    {
        return renderPass;
    }
    void release()override;
private:
    VK_ContextImpl *context = nullptr;
    VkRenderPass renderPass = nullptr;
};

#endif // VK_RENDERPASS_H
