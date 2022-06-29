#ifndef VK_UTIL_H
#define VK_UTIL_H
#include <string>
#include <cstring>
#include <vector>
#include <optional>
#include <vulkan/vulkan.h>

inline bool hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

template<class T>
inline bool isSame(const std::vector<T> &a, const std::vector<T> &b)
{
    if (a.size() != b.size())
        return false;

    int size = sizeof(T) * a.size();
    return !memcmp((char *)a.data(), (char *)b.data(), size);
}

template<class C>
void cleanVulkanObjectContainer(C &container)
{
    while (true) {
        auto itr = container.begin();
        if (itr == container.end())
            break;
        else
            (*itr)->release();
    }
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

std::vector<char> readDataFromFile(const std::string &filename);

[[nodiscard]] VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physicalDevice);

VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice,
                             const std::vector<VkFormat> &candidates,
                             VkImageTiling tiling,
                             VkFormatFeatureFlags features);
VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties);

void printUUID(uint8_t *pipelineCacheUUID);

void adjustImageLayout(VkCommandBuffer command, VkImage image, VkImageLayout oldLayout,
                       VkImageLayout newLayout, uint32_t levelCount = VK_REMAINING_MIP_LEVELS);

void writePpm(const std::string &filename, uint32_t width, uint32_t height, VkFormat format,
              uint32_t rowPitch, char *ptr);

class VK_Context;
class VK_Image;
void writeFile(VK_Context *context, const std::string &file, VkImage image, uint32_t width,
               uint32_t height);

#endif // VK_UTIL_H
