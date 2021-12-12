#include <iostream>
#include "VK_SamplerImpl.h"

VkSamplerCreateInfo VK_Sampler::createSamplerCreateInfo(uint32_t mipLevels)
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE;//VK_TRUE;
    samplerInfo.maxAnisotropy = 1.0f;
    //deviceProperties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0;
    samplerInfo.maxLod = static_cast<float>(mipLevels);
    samplerInfo.mipLodBias = 0;
    return samplerInfo;
}

VK_SamplerImpl::VK_SamplerImpl(VkDevice vkDevice, VK_ContextImpl* vkContext):
    device(vkDevice),
    context(vkContext)
{
}

VK_SamplerImpl::~VK_SamplerImpl()
{
}

void VK_SamplerImpl::release()
{
    if(isRemoveFromContainerWhenRelease())
        context->removeSampler(this);

    if(textureSampler)
        vkDestroySampler(device, textureSampler, nullptr);

    delete this;
}

VkSampler VK_SamplerImpl::getSampler() const
{
    return textureSampler;
}

bool VK_SamplerImpl::create(const VkSamplerCreateInfo& samplerInfo)
{
    if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        std::cerr << "failed to create texture sampler!" << std::endl;
        return false;
    }

    return true;
}
