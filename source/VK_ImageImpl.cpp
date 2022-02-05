#include <cstring>
#include <iostream>
#include <cmath>
#include <stb_image.h>
#include "VK_ImageImpl.h"
#include "VK_ContextImpl.h"

VK_ImageImpl::VK_ImageImpl(VK_ContextImpl *vkContext):
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
    context->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                          stagingBufferMemory);

    void *data = nullptr;
    vkMapMemory(context->getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(context->getDevice(), stagingBufferMemory);

    stbi_image_free(pixels);

    mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

    createImage(width, height, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
    copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(width),
                      static_cast<uint32_t>(height));

    vkDestroyBuffer(context->getDevice(), stagingBuffer, context->getAllocation());
    vkFreeMemory(context->getDevice(), stagingBufferMemory, context->getAllocation());
    generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_UNORM, width, height, mipLevels);

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
    if (isRemoveFromContainerWhenRelease())
        context->onReleaseImage(this);

    if (textureImage)
        vkDestroyImage(context->getDevice(), textureImage, context->getAllocation());
    if (textureImageMemory)
        vkFreeMemory(context->getDevice(), textureImageMemory, context->getAllocation());
    delete this;
}

void VK_ImageImpl::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth,
                                   int32_t texHeight, uint32_t mipLevels)
{
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(context->getPhysicalDevice(), imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        std::cerr << "texture image format does not support linear blitting!" << std::endl;
    }

    VkCommandBuffer commandBuffer = context->getCommandPool()->beginSingleTimeCommands();

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        VkImageBlit blit = {};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer,
                       image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &blit,
                       VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        if (mipWidth > 1)
            mipWidth /= 2;
        if (mipHeight > 1)
            mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);

    context->getCommandPool()->endSingleTimeCommands(commandBuffer, context->getGraphicQueue());
}

void VK_ImageImpl::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                                     uint32_t height)
{
    VkCommandBuffer commandBuffer = context->getCommandPool()->beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &region);
    context->getCommandPool()->endSingleTimeCommands(commandBuffer, context->getGraphicQueue());
}

void VK_ImageImpl::transitionImageLayout(VkImageLayout oldLayout,
        VkImageLayout newLayout, uint32_t mipLevels)
{
    VkCommandBuffer commandBuffer = context->getCommandPool()->beginSingleTimeCommands();
    adjustImageLayout(commandBuffer, textureImage, oldLayout, newLayout, mipLevels);
    context->getCommandPool()->endSingleTimeCommands(commandBuffer, context->getGraphicQueue());
}

bool VK_ImageImpl::createImage(uint32_t width, uint32_t height, VkSampleCountFlagBits sample,
                               VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
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

    if (vkCreateImage(context->getDevice(), &createInfo, context->getAllocation(),
                      &textureImage) != VK_SUCCESS) {
        std::cerr << "failed to create image!" << std::endl;
        return true;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(context->getDevice(), textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(context->getPhysicalDevice(),
                                memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(context->getDevice(), &allocInfo, context->getAllocation(),
                         &textureImageMemory) != VK_SUCCESS) {
        std::cerr << "failed to allocate image memory!" << std::endl;
        return false;
    }

    vkBindImageMemory(context->getDevice(), textureImage, textureImageMemory, 0);
    return true;
}

