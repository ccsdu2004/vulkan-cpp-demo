#include "VK_DescriptorSetLayout.h"
#include "VK_ContextImpl.h"
#include "VK_ShaderSet.h"

VK_DescriptorSetLayout::VK_DescriptorSetLayout(VK_ContextImpl *vkContext, VK_ShaderSet *shaderSet):
    context(vkContext)
{
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = shaderSet->getDescriptorSetLayoutBindingCount();
    layoutInfo.pBindings = shaderSet->getDescriptorSetLayoutBindingData();
    layoutInfo.pNext = nullptr;

    if (vkCreateDescriptorSetLayout(context->getDevice(), &layoutInfo, context->getAllocation(),
                                    &descriptorSetLayout) != VK_SUCCESS) {
        std::cerr << "failed to create descriptor set layout!" << std::endl;
    }
}

VK_DescriptorSetLayout::~VK_DescriptorSetLayout()
{
}

VkDescriptorSetLayout VK_DescriptorSetLayout::getDescriptorSetLayout() const
{
    return descriptorSetLayout;
}

void VK_DescriptorSetLayout::release()
{
    vkDestroyDescriptorSetLayout(context->getDevice(), descriptorSetLayout, context->getAllocation());
    descriptorSetLayout = nullptr;
    delete this;
}
