#include <VK_PushDescriptor.h>
#include <VK_Context.h>

VK_PushDescriptor::VK_PushDescriptor(VK_Context *vkContext):
    context(vkContext)
{
    vkCmdPushDescriptorSetKHR = (PFN_vkCmdPushDescriptorSetKHR)vkGetDeviceProcAddr(context->getDevice(), "vkCmdPushDescriptorSetKHR");
}

void VK_PushDescriptor::addDescriptor(const VkWriteDescriptorSet &descriptor)
{
    descriptors.push_back(descriptor);
}

void VK_PushDescriptor::push(VkCommandBuffer commandBuffer, VkPipelineLayout layout)
{
    if(vkCmdPushDescriptorSetKHR)
        vkCmdPushDescriptorSetKHR(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, descriptors.size(), descriptors.data());
}
