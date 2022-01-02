#include <cstring>
#include <iostream>
#include <cmath>
#include <stb_image.h>
#include "VK_ImageImpl.h"
#include "VK_ContextImpl.h"

VK_ImageImpl::VK_ImageImpl(VkDevice vkDevice, VK_ContextImpl *vkContext):
    device(vkDevice),
    context(vkContext)
{
}

VK_ImageImpl::~VK_ImageImpl()
{

}

bool VK_ImageImpl::load(const std::string &filename)
{
    int width, height, texChannels;
    auto pixels = stbi_load(filename.data(), &width, &height, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = width * height * 4;

    if (!pixels) {
        std::cerr << "failed to load texture image!" << std::endl;
        return false;
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    context->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data = nullptr;
    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(device, stagingBufferMemory);

    stbi_image_free(pixels);

    mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

    createImage(width, height, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    context->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
    context->copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

    vkDestroyBuffer(device, stagingBuffer, context->getAllocation());
    vkFreeMemory(device, stagingBufferMemory, context->getAllocation());
    context->generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_UNORM, width, height, mipLevels);

    return true;
}

VkImage VK_ImageImpl::getImage() const
{
    return textureImage;
}

int VK_ImageImpl::getWidth() const
{
    return createInfo.extent.width;
}

int VK_ImageImpl::getHeight() const
{
    return createInfo.extent.height;
}

int VK_ImageImpl::getMipLevel() const
{
    return mipLevels;
}

void VK_ImageImpl::release()
{
    if(isRemoveFromContainerWhenRelease())
        context->onReleaseImage(this);

    if(textureImage)
        vkDestroyImage(device, textureImage, context->getAllocation());
    if(textureImageMemory)
        vkFreeMemory(device, textureImageMemory, context->getAllocation());
    delete this;
}

bool VK_ImageImpl::createImage(uint32_t width, uint32_t height, VkSampleCountFlagBits sample, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
{
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent.width = width;
    createInfo.extent.height = height;
    createInfo.extent.depth = 1;
    createInfo.mipLevels = mipLevels;
    createInfo.arrayLayers = 1;
    createInfo.format = format;
    createInfo.tiling = tiling;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage = usage;
    createInfo.samples = sample;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.pNext = nullptr;

    if (vkCreateImage(device, &createInfo, context->getAllocation(), &textureImage) != VK_SUCCESS) {
        std::cerr << "failed to create image!" << std::endl;
        return true;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = context->findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, context->getAllocation(), &textureImageMemory) != VK_SUCCESS) {
        std::cerr << "failed to allocate image memory!" << std::endl;
        return false;
    }

    vkBindImageMemory(device, textureImage, textureImageMemory, 0);
    return true;
}

