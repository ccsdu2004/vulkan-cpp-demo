#ifndef VK_IMAGE_HPP
#define VK_IMAGE_HPP
#include <vulkan/vulkan.h>
#include "VK_Deleter.h"

class VK_Image : public VK_Deleter
{
public:
    virtual VkImage getImage()const = 0;
    virtual int getWidth()const = 0;
    virtual int getHeight()const = 0;
    virtual int getMipLevel()const = 0;
    virtual void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) = 0;
};

#endif // VK_IMAGE_HPP
