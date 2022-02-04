#ifndef VK_IMAGEVIEWIMPL_H
#define VK_IMAGEVIEWIMPL_H
#include "VK_ImageView.h"

class VK_ContextImpl;

class VK_ImageViewImpl : public VK_ImageView
{
public:
    VK_ImageViewImpl(VK_ContextImpl* vkContext);
    ~VK_ImageViewImpl();
public:
    bool create(const VkImageViewCreateInfo& viewCreateInfo, uint32_t mipLevels);
    void setSampler(VkSampler sampler)override;
    void release()override;
public:
    VkWriteDescriptorSet createWriteDescriptorSet(VkDescriptorSet descriptorSet)const override;
    VkImageView getImageView()const override;
private:
    VK_ContextImpl* context = nullptr;

    VkDescriptorImageInfo imageInfo;
    VkImageView textureImageView = 0;
};

#endif // VK_IMAGEVIEWIMPL_H
