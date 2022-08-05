#ifndef VK_DESCRIPTORSETS_H
#define VK_DESCRIPTORSETS_H
#include <vector>
#include <list>
#include <map>
#include <vulkan/vulkan.h>
#include "VK_ImageView.h"

class VK_Buffer;
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
                const std::map<VK_ImageView *, uint32_t> &imageViews);

    void bind(VkCommandBuffer command, VkPipelineLayout pipelineLayout, uint32_t index);
    void render(VkCommandBuffer command, VkPipelineLayout pipelineLayout, uint32_t index,
                const std::list<VK_Buffer *> &buffers);
private:
    VK_ContextImpl *context = nullptr;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<std::vector<uint32_t>> dynamicOffsets;
};

#endif // VK_DESCRIPTORSETS_H
