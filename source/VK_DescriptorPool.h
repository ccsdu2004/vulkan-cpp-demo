#ifndef VK_DESCRIPTORPOOL_H
#define VK_DESCRIPTORPOOL_H
#include <vulkan/vulkan.h>
#include <VK_Deleter.h>

class VK_ShaderSet;
class VK_ContextImpl;

class VK_DescriptorPool : public VK_Deleter
{
public:
    VK_DescriptorPool(VK_ContextImpl *vkContext);
    ~VK_DescriptorPool() {}
public:
    void create(VK_ShaderSet *shader);
    VkDescriptorPool getDescriptorPool()const;
    void release()override;
private:
    VK_ContextImpl *context = nullptr;
    VkDescriptorPool descriptorPool = nullptr;
};

#endif // VK_DESCRIPTORPOOL_H
