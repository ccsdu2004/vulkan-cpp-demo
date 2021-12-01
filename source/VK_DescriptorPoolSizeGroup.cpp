#include "VK_DescriptorPoolSizeGroup.h"

void VK_VkDescriptorPoolSizeGroup::addDescriptorPoolSize(VkDescriptorType type)
{
    VkDescriptorPoolSize poolSize;
    poolSize.type = type;
    poolSize.descriptorCount = 0;
    addDescriptorPoolSize(poolSize);
}

void VK_VkDescriptorPoolSizeGroup::addDescriptorPoolSize(VkDescriptorPoolSize poolSize)
{
    descriptorPoolSize.push_back(poolSize);
}

void VK_VkDescriptorPoolSizeGroup::update(int32_t size)
{
    auto itr = descriptorPoolSize.begin();
    while(itr != descriptorPoolSize.end()) {
        (*itr).descriptorCount = size;
        itr ++;
    }
}

size_t VK_VkDescriptorPoolSizeGroup::getCount()const
{
    return descriptorPoolSize.size();
}

const VkDescriptorPoolSize* VK_VkDescriptorPoolSizeGroup::getData()const
{
    return descriptorPoolSize.data();
}
