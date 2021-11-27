#include "VK_DescriptorSetLayoutBindingGroup.h"


size_t VK_DescriptorSetLayoutBindingGroup::getCount() const
{
    return bindings.size();
}

const VkDescriptorSetLayoutBinding *VK_DescriptorSetLayoutBindingGroup::getData() const
{
    return bindings.data();
}

VkDescriptorSetLayoutBinding VK_DescriptorSetLayoutBindingGroup::createDescriptorSetLayoutBinding(uint32_t id, VkDescriptorType type, VkShaderStageFlagBits flag)
{
    VkDescriptorSetLayoutBinding binding;
    binding.binding = id;
    binding.descriptorCount = 1;
    binding.descriptorType = type;
    binding.pImmutableSamplers = nullptr;
    binding.stageFlags = flag;
    return binding;
}

void VK_DescriptorSetLayoutBindingGroup::addDescriptorSetLayoutBinding(const VkDescriptorSetLayoutBinding &binding)
{
    bindings.push_back(binding);
}
