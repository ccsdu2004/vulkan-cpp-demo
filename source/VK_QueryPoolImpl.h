#pragma once
#include <vulkan/vulkan.h>
#include <VK_Context.h>
#include <VK_QueryPool.h>

class VK_QueryPoolImpl : public VK_QueryPool
{
public:
    VK_QueryPoolImpl() = delete;
    VK_QueryPoolImpl(VK_Context *inputContext, uint32_t count, VkQueryPipelineStatisticFlags flag);
public:
    void release() override;
    void reset(VkCommandBuffer commandBuffer) override;
    void startQeury(VkCommandBuffer commandBuffer) override;
    void endQuery(VkCommandBuffer commandBuffer) override;
    void setQueryCallback(std::function<void(const std::vector<uint64_t>& data)> fn) override;

    void query();
private:
    VK_Context *context = nullptr;
    VkQueryPool queryPool = nullptr;
    std::vector<uint64_t> queryData;
    std::function<void(const std::vector<uint64_t>& data)> queryCallback;
};
