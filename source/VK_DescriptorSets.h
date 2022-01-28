#ifndef VK_DESCRIPTORSETS_H
#define VK_DESCRIPTORSETS_H
#include <vector>
#include <list>
#include <vulkan/vulkan.h>
#include "VK_ImageView.h"

class VK_UniformBuffer;
class VK_ContextImpl;

class VK_DescriptorSets
{
public:
    VK_DescriptorSets() = delete;
    VK_DescriptorSets(VK_ContextImpl *vkContext);
    ~VK_DescriptorSets();
public:
    void init(VkDescriptorPool pool, VkDescriptorSetLayout setLayout);
    void update(const std::list<VK_UniformBuffer *> &uniformBuffers,
                const std::list<VK_ImageView *> &imageViews);

    void bind(VkCommandBuffer command, VkPipelineLayout pipelineLayout, uint32_t index);
private:
    VK_ContextImpl *context = nullptr;
    std::vector<VkDescriptorSet> descriptorSets;
};

#endif // VK_DESCRIPTORSETS_H
