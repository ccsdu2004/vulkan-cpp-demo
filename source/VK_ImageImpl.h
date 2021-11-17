#ifndef VK_IMAGEIMPI_H
#define VK_IMAGEIMPI_H
#include <string>
#include <vulkan/vulkan.h>
#include "VK_Image.hpp"
#include "VK_Context.h"

class VK_ImageImpl : public VK_Image
{
public:
    VK_ImageImpl();
    ~VK_ImageImpl();
public:
    bool load(const std::string& filename);
    void release()override;
private:
    bool createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
private:
    VkDevice device = nullptr;
    VK_Context* context = nullptr;
    VkImage textureImage = 0;
    VkDeviceMemory textureImageMemory = 0;
};

#endif // VK_IMAGEIMP_H
