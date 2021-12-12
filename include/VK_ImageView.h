#ifndef VK_IMAGEVIEW_H
#define VK_IMAGEVIEW_H
#include <vulkan/vulkan.h>
#include "VK_Deleter.h"

class VK_ImageView : public VK_Deleter
{
public:
    static VkImageViewCreateInfo createImageViewCreateInfo(VkImage image, VkFormat format,uint32_t mipLevels = 1);

    virtual void setSampler(VkSampler sampler) = 0;
    virtual VkWriteDescriptorSet createWriteDescriptorSet(VkDescriptorSet descriptorSet)const = 0;
    virtual VkImageView getImageView()const = 0;
};

#endif // VK_IMAGEVIEW_H
