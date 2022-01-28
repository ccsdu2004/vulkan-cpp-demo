#ifndef VK_DESCRIPTORSETLAYOUT_H
#define VK_DESCRIPTORSETLAYOUT_H
#include <vulkan/vulkan.h>
#include "VK_Deleter.h"

class VK_ShaderSet;
class VK_ContextImpl;

class VK_DescriptorSetLayout : public VK_Deleter
{
public:
    VK_DescriptorSetLayout() = delete;
    VK_DescriptorSetLayout(VK_ContextImpl *vkContext, VK_ShaderSet *shaderSet);
    ~VK_DescriptorSetLayout();
public:
    VkDescriptorSetLayout getDescriptorSetLayout()const;
    void release()override;
private:
    VK_ContextImpl *context = nullptr;
    VkDescriptorSetLayout descriptorSetLayout = nullptr;
};

#endif // VK_DESCRIPTORSETLAYOUT_H
