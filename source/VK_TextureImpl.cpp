#include <iostream>
#include "VK_TextureImpl.h"

VK_TextureImpl::VK_TextureImpl(VkDevice vkDevice, VK_ContextImpl* vkContext):
    device(vkDevice),
    context(vkContext)
{
}

VK_TextureImpl::~VK_TextureImpl()
{
}

void VK_TextureImpl::release()
{
    if(isRemoveFromContainerWhenRelease())
        context->onReleaseTexture(this);

    if(textureSampler)
        vkDestroySampler(device, textureSampler, nullptr);

    if(textureImageView)
        vkDestroyImageView(device, textureImageView, nullptr);

    delete this;
}

VkImageView VK_TextureImpl::getTextureImageView() const
{
    return textureImageView;
}

bool VK_TextureImpl::create(const VkImageViewCreateInfo& viewCreateInfo, const VkSamplerCreateInfo& samplerInfo)
{
    if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        std::cerr << "failed to create texture sampler!" << std::endl;
        return false;
    }

    if (vkCreateImageView(device, &viewCreateInfo, nullptr, &textureImageView) != VK_SUCCESS) {
        std::cerr << "failed to create texture image view!" << std::endl;
        return false;
    }
    return true;
}

