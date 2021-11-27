#ifndef VK_DESCRIPTORSETLAYOUTBINDINGGROUP_H
#define VK_DESCRIPTORSETLAYOUTBINDINGGROUP_H
#include <vector>
#include <vulkan/vulkan.h>

class VK_DescriptorSetLayoutBindingGroup
{
public:
    VK_DescriptorSetLayoutBindingGroup() = default;
    ~VK_DescriptorSetLayoutBindingGroup() = default;
public:
    size_t getCount()const;
    const VkDescriptorSetLayoutBinding* getData()const;

    static VkDescriptorSetLayoutBinding createDescriptorSetLayoutBinding(uint32_t id, VkDescriptorType type, VkShaderStageFlagBits flag);
    void addDescriptorSetLayoutBinding(const VkDescriptorSetLayoutBinding& bding);
private:
    std::vector<VkDescriptorSetLayoutBinding> bindings;
};

#endif // VK_DESCRIPTORSETLAYOUTBINDINGGROUP_H
