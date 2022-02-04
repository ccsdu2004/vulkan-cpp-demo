#include "VK_ImageViewImpl.h"
#include "VK_ContextImpl.h"

VK_ImageViewImpl::VK_ImageViewImpl(VK_ContextImpl *vkContext):
    context(vkContext)
{
}

VK_ImageViewImpl::~VK_ImageViewImpl()
{
}

void VK_ImageViewImpl::release()
{
    if (isRemoveFromContainerWhenRelease())
        context->removeImageView(this);

    if (textureImageView)
        vkDestroyImageView(context->getDevice(), textureImageView, context->getAllocation());

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

bool VK_ImageViewImpl::create(const VkImageViewCreateInfo &viewCreateInfo, uint32_t mipLevels)
{
    if (vkCreateImageView(context->getDevice(), &viewCreateInfo, context->getAllocation(),
                          &textureImageView) != VK_SUCCESS) {
        std::cerr << "failed to create texture image view!" << std::endl;
        return false;
    }

    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureImageView;
    imageInfo.sampler = nullptr;
    return true;
}

void VK_ImageViewImpl::setSampler(VkSampler sampler)
{
    imageInfo.sampler = sampler;
}
