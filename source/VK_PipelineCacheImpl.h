#ifndef VK_PIPELINECACHEIMPL_H
#define VK_PIPELINECACHEIMPL_H
#include <VK_PipelineCache.h>

class VK_PipelineCacheImpl : public VK_PipelineCache
{
public:
    VK_PipelineCacheImpl() = delete;
    VK_PipelineCacheImpl(VkDevice device,
                         VkAllocationCallbacks* allocator,
                         VkPhysicalDeviceProperties properties);
public:
    bool create(const std::string& file, bool debug);
    VkPipelineCache getPipelineCache()const override
    {
        return vkPipelineCache;
    }
    void release() override;
    bool saveGraphicsPiplineCache(const std::string &file) override;
    bool isValidCacheData(const std::string &filename, const char *buffer, uint32_t size) override;
private:
    VkDevice vkDevice = nullptr;
    VkPipelineCache vkPipelineCache = nullptr;
    VkAllocationCallbacks* vkAllocator = nullptr;
    VkPhysicalDeviceProperties vkPhysicalDeviceProperties;
};

#endif // VK_PIPELINECACHEIMPL_H
