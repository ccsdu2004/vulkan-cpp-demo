#include "VK_ShaderSet.h"

VkDescriptorSetLayoutBinding VK_ShaderSet::createDescriptorSetLayoutBinding(uint32_t id, VkDescriptorType type, VkShaderStageFlagBits flag)
{
    VkDescriptorSetLayoutBinding binding;
    binding.binding = id;
    binding.descriptorCount = 1;
    binding.descriptorType = type;
    binding.pImmutableSamplers = nullptr;
    binding.stageFlags = flag;
    return binding;
}
