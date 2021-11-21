#ifndef VK_TEXTURE_H
#define VK_TEXTURE_H
#include <vulkan/vulkan.h>
#include "VK_Deleter.h"

class VK_Texture : public VK_Deleter
{
public:
    virtual VkImageView getTextureImageView()const = 0;
};

#endif // VK_TEXTURE_H
