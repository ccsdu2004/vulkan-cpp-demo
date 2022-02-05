#include <fstream>
#include <iostream>
#include <iomanip>
#include "VK_Util.h"

std::vector<char> readDataFromFile(const std::string &filename)
{
    std::vector<char> buffer;

    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "failed to open file:" << filename.data() << std::endl;
        return buffer;
    }

    size_t size = (size_t) file.tellg();
    buffer.resize(size);

    file.seekg(0);
    file.read(buffer.data(), size);
    file.close();
    return buffer;
}

VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
                                physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) {
        return VK_SAMPLE_COUNT_64_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_32_BIT) {
        return VK_SAMPLE_COUNT_32_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_16_BIT) {
        return VK_SAMPLE_COUNT_16_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_8_BIT) {
        return VK_SAMPLE_COUNT_8_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_4_BIT) {
        return VK_SAMPLE_COUNT_4_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_2_BIT) {
        return VK_SAMPLE_COUNT_2_BIT;
    }

    return VK_SAMPLE_COUNT_1_BIT;
}

VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice,
                             const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL
                   && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    std::cerr << "failed to find supported format!" << std::endl;
    return VkFormat();
}

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice)
{
    std::vector<VkFormat> list = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    return findSupportedFormat(physicalDevice, list, VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i))
            && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    std::cerr << "failed to find suitable memory type!" << std::endl;
    return ~0;
}

void printUUID(uint8_t *pipelineCacheUUID)
{
    for (size_t j = 0; j < VK_UUID_SIZE; ++j) {
        std::cout << std::setw(2) << (uint32_t)pipelineCacheUUID[j];
        if (j == 3 || j == 5 || j == 7 || j == 9) {
            std::cout << '-';
        }
    }
}

void writePpm(const std::string& filename, uint32_t width, uint32_t height, VkFormat format, uint32_t rowPitch, char* ptr)
{
    std::ofstream file(filename.c_str(), std::ios::binary);

    file << "P6\n";
    file << width << " ";
    file << height << "\n";
    file << 255 << "\n";

    for (size_t y = 0; y < height; y++) {
        const int *row = (const int *)ptr;
        int swapped;

        if (format == VK_FORMAT_B8G8R8A8_UNORM || format == VK_FORMAT_B8G8R8A8_SRGB) {
            for (size_t x = 0; x < width; x++) {
                swapped = (*row & 0xff00ff00) | (*row & 0x000000ff) << 16 | (*row & 0x00ff0000) >> 16;
                file.write((char *)&swapped, 3);
                row++;
            }
        } else if (format == VK_FORMAT_R8G8B8A8_UNORM) {
            for (size_t x = 0; x < width; x++) {
                file.write((char *)row, 3);
                row++;
            }
        } else {
            printf("unrecognized image format - will not write image files");
            break;
        }

        ptr += rowPitch;
    }

    file.close();
}

void adjustImageLayout(VkCommandBuffer command, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t levelCount)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = levelCount;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
               && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
               && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }

    vkCmdPipelineBarrier(
        command,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
}
