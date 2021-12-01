#include "VK_ImageViewImpl.h"
#include "VK_ContextImpl.h"

VK_ImageViewImpl::VK_ImageViewImpl(VkDevice vkDevice, VK_ContextImpl* vkContext):
    device(vkDevice),
    context(vkContext)
{
}

VK_ImageViewImpl::~VK_ImageViewImpl()
{
}

void VK_ImageViewImpl::release()
{
    if(isRemoveFromContainerWhenRelease())
        context->removeImageView(this);

    if(textureImageView)
        vkDestroyImageView(device, textureImageView, nullptr);

    delete this;
}

VkWriteDescriptorSet VK_ImageViewImpl::createWriteDescriptorSet(VkDescriptorSet descriptorSet) const
{
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 1;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    return descriptorWrite;
}

VkImageView VK_ImageViewImpl::getImageView() const
{
    return textureImageView;
}

bool VK_ImageViewImpl::create(const VkImageViewCreateInfo& viewCreateInfo)
{
    if (vkCreateImageView(device, &viewCreateInfo, nullptr, &textureImageView) != VK_SUCCESS) {
        std::cerr << "failed to create texture image view!" << std::endl;
        return false;
    }

    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureImageView;
    imageInfo.sampler = 0;
    return true;
}

void VK_ImageViewImpl::setSampler(VkSampler sampler)
{
    imageInfo.sampler = sampler;
}
