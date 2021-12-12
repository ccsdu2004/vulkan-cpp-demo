#ifndef VK_IMAGEIMPI_H
#define VK_IMAGEIMPI_H
#include <string>
#include <vulkan/vulkan.h>
#include "VK_Image.h"

class VK_ContextImpl;

class VK_ImageImpl : public VK_Image
{
public:
    VK_ImageImpl(VkDevice vkDevice, VK_ContextImpl* vkContext);
    ~VK_ImageImpl();
public:
    bool load(const std::string& filename);
    bool createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

    VkImage getImage()const override;
    int getWidth()const override;
    int getHeight()const override;
    int getMipLevel()const override;

    void release()override;
private:
    VkDevice device = nullptr;
    VK_ContextImpl* context = nullptr;
    VkImageCreateInfo createInfo = {};
    VkImage textureImage = 0;
    VkDeviceMemory textureImageMemory = 0;
    uint32_t mipLevels = 1;
};

#endif // VK_IMAGEIMP_H
