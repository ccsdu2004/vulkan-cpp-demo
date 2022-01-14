#ifndef VK_PIPELINECACHE_H
#define VK_PIPELINECACHE_H
#include <string>
#include <vulkan/vulkan.h>
#include <VK_Deleter.h>

class VK_PipelineCache : public VK_Deleter
{
public:
    virtual VkPipelineCache getPipelineCache()const = 0;
    virtual bool saveGraphicsPiplineCache(const std::string& file) = 0;
    virtual bool isValidCacheData(const std::string &filename, const char *buffer,
                                  uint32_t size) = 0;
};

#endif // VK_PIPELINECACHE_H
