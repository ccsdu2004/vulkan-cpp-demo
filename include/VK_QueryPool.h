#ifndef VK_QUERYPOOL_H
#define VK_QUERYPOOL_H
#include <vector>
#include <functional>
#include <vulkan/vulkan.h>
#include <VK_Deleter.h>

class VK_QueryPool : public VK_Deleter
{
public:
    virtual void reset(VkCommandBuffer commandBuffer) = 0;
    virtual void startQeury(VkCommandBuffer commandBuffer) = 0;
    virtual void endQuery(VkCommandBuffer commandBuffer) = 0;
    virtual void setQueryCallback(std::function<void(const std::vector<uint64_t>& data)> fn) = 0;
};

#endif // VK_QUERYPOOL_H
