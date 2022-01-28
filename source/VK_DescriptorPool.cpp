#include <VK_DescriptorPool.h>
#include <VK_ContextImpl.h>

VK_DescriptorPool::VK_DescriptorPool(VK_ContextImpl *vkContext):
    context(vkContext)
{

}

void VK_DescriptorPool::create(VK_ShaderSet *shaderSet)
{
    shaderSet->updateDescriptorPoolSize(context->getSwapImageCount());

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = context->getSwapImageCount();

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    if (shaderSet->getDescriptorPoolSizeCount() > 0) {
        poolInfo.poolSizeCount = shaderSet->getDescriptorPoolSizeCount();
        poolInfo.pPoolSizes = shaderSet->getDescriptorPoolSizeData();
    } else {
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
    }
    poolInfo.maxSets = context->getSwapImageCount();

    if (vkCreateDescriptorPool(context->getDevice(), &poolInfo, context->getAllocation(),
                               &descriptorPool) != VK_SUCCESS) {
        std::cerr << "failed to create descriptor pool!" << std::endl;
    }
}

VkDescriptorPool VK_DescriptorPool::getDescriptorPool() const{return descriptorPool;}

void VK_DescriptorPool::release()
{
    vkDestroyDescriptorPool(context->getDevice(), descriptorPool, context->getAllocation());
}
