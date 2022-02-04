#include <memory.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include "VK_PipelineCacheImpl.h"
#include "VK_Util.h"

VK_PipelineCacheImpl::VK_PipelineCacheImpl(VkDevice device,
        VkAllocationCallbacks* allocator,
        VkPhysicalDeviceProperties properties):
    vkDevice(device),
    vkAllocator(allocator),
    vkPhysicalDeviceProperties(properties)
{
}

bool VK_PipelineCacheImpl::create(const std::string &file, bool debug)
{
    if(vkPipelineCache)
        return true;

    auto buffer = readDataFromFile(file);
    bool valid = debug && isValidCacheData(file, buffer.data(), buffer.size());

    VkPipelineCacheCreateInfo PipelineCacheCreateInfo = {};
    PipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    PipelineCacheCreateInfo.pNext = nullptr;
    PipelineCacheCreateInfo.initialDataSize = valid ? buffer.size() : 0;
    PipelineCacheCreateInfo.pInitialData = valid ? buffer.data() : nullptr;

    if (vkCreatePipelineCache(vkDevice, &PipelineCacheCreateInfo, vkAllocator, &vkPipelineCache) != VK_SUCCESS) {
        std::cerr << "creating pipeline cache error" << std::endl;
        return false;
    }
    return true;
}

void VK_PipelineCacheImpl::release()
{
    vkDestroyPipelineCache(vkDevice, vkPipelineCache, vkAllocator);
    vkPipelineCache = nullptr;
    delete this;
}

bool VK_PipelineCacheImpl::saveGraphicsPiplineCache(const std::string &file)
{
    size_t cacheSize = 0;

    if (vkGetPipelineCacheData(vkDevice, vkPipelineCache, &cacheSize, nullptr) != VK_SUCCESS) {
        std::cerr << "getting cache size fail from pipelinecache" << std::endl;
        return false;
    }

    auto cacheData = std::vector<char>(sizeof(char) * cacheSize, 0);

    if (vkGetPipelineCacheData(vkDevice, vkPipelineCache, &cacheSize, &cacheData[0]) != VK_SUCCESS) {
        std::cerr << "getting cache fail from pipelinecache" << std::endl;
        return false;
    }

    std::ofstream stream(file, std::ios::binary);
    if (stream.is_open()) {
        stream.write(cacheData.data(), cacheData.size());
        stream.close();
    } else {
        std::cerr << "open pipeline cache data target file failed!" << std::endl;
        return false;
    }
    return true;
}

bool VK_PipelineCacheImpl::isValidCacheData(const std::string &filename, const char *buffer, uint32_t size)
{
    if (size > 32) {
        uint32_t header = 0;
        uint32_t version = 0;
        uint32_t vendor = 0;
        uint32_t deviceID = 0;
        uint8_t pipelineCacheUUID[VK_UUID_SIZE] = {};

        memcpy(&header, (uint8_t *)buffer + 0, 4);
        memcpy(&version, (uint8_t *)buffer + 4, 4);
        memcpy(&vendor, (uint8_t *)buffer + 8, 4);
        memcpy(&deviceID, (uint8_t *)buffer + 12, 4);
        memcpy(pipelineCacheUUID, (uint8_t *)buffer + 16, VK_UUID_SIZE);

        if (header <= 0) {
            std::cerr << "bad pipeline cache data header length in " << filename << std::endl;
            return false;
        }

        if (version != VK_PIPELINE_CACHE_HEADER_VERSION_ONE) {
            std::cerr << "unsupported cache header version in " << filename << std::endl;
            std::cerr << "cache contains: 0x" << std::hex << version << std::endl;
        }

        if (vendor != vkPhysicalDeviceProperties.vendorID) {
            std::cerr << "vendor id mismatch in " << filename << std::endl;
            std::cerr << "cache contains: 0x" << std::hex << vendor << std::endl;
            std::cerr << "driver expects: 0x" << vkPhysicalDeviceProperties.vendorID << std::endl;
            return false;
        }

        if (deviceID != vkPhysicalDeviceProperties.deviceID) {
            std::cerr << "device id mismatch in " << filename << std::endl;
            std::cerr << "cache contains: 0x" << std::hex << deviceID << std::endl;
            std::cerr << "driver expects: 0x" << vkPhysicalDeviceProperties.deviceID << std::endl;
            return false;
        }

        if (memcmp(pipelineCacheUUID, vkPhysicalDeviceProperties.pipelineCacheUUID, sizeof(pipelineCacheUUID)) != 0) {
            std::cerr << "uuid mismatch in " << filename << std::endl;
            std::cerr << "cache contains: " << std::endl;

            printUUID(pipelineCacheUUID);

            std::cerr << "driver expects:" << std::endl;
            printUUID(vkPhysicalDeviceProperties.pipelineCacheUUID);
        };

        return true;
    }
    return false;
}
