#ifndef PUSHDESCRIPTOR_H
#define PUSHDESCRIPTOR_H
#include <vector>
#include <vulkan/vulkan.h>

class VK_Context;

class VK_PushDescriptor
{
public:
    VK_PushDescriptor() = delete;
    VK_PushDescriptor(VK_Context* vkContext);
public:
    void addDescriptor(const VkWriteDescriptorSet& descriptor);
    void push(VkCommandBuffer commandBuffer, VkPipelineLayout layout);
private:
    VK_Context* context = nullptr;
    PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR = nullptr;
    std::vector<VkWriteDescriptorSet> descriptors;
};

#endif // PUSHDESCRIPTOR_H
