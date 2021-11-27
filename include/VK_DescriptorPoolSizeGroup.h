#ifndef VK_DESCRIPTORPOOLSIZEGROUP_H
#define VK_DESCRIPTORPOOLSIZEGROUP_H
#include <vector>
#include <vulkan/vulkan.h>

class VK_VkDescriptorPoolSizeGroup
{
public:
    VK_VkDescriptorPoolSizeGroup() = default;
    ~VK_VkDescriptorPoolSizeGroup() = default;
public:
    void addDescriptorPoolSize(VkDescriptorType type);
    void addDescriptorPoolSize(VkDescriptorPoolSize poolSize);

    void update(int32_t size);

    size_t getCount()const;
    const VkDescriptorPoolSize *getData()const;
private:
    std::vector<VkDescriptorPoolSize> descriptorPoolSize;
};

#endif // VK_DESCRIPTORPOOLSIZEGROUP_H
