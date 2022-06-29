#include <VK_QueryPoolImpl.h>

VK_QueryPoolImpl::VK_QueryPoolImpl(VK_Context *inputContext,
                                   uint32_t count, VkQueryPipelineStatisticFlags flag):
    context(inputContext)
{
    VkQueryPoolCreateInfo queryPoolCreateInfo{};
    queryPoolCreateInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
    queryPoolCreateInfo.queryType  = VK_QUERY_TYPE_PIPELINE_STATISTICS;
    queryPoolCreateInfo.queryCount = count;
    queryPoolCreateInfo.pipelineStatistics = flag;
    auto status = vkCreateQueryPool(context->getDevice(), &queryPoolCreateInfo,
                                    context->getAllocation(),
                                    &queryPool);
    assert(status == VK_SUCCESS);

    queryData.resize(count);
}

void VK_QueryPoolImpl::release()
{
    vkDestroyQueryPool(context->getDevice(), queryPool, context->getAllocation());
}

void VK_QueryPoolImpl::reset(VkCommandBuffer commandBuffer)
{
    vkCmdResetQueryPool(commandBuffer, queryPool, 0, queryData.size());
}

void VK_QueryPoolImpl::startQeury(VkCommandBuffer commandBuffer)
{
    vkCmdBeginQuery(commandBuffer, queryPool, 0, 0);
}

void VK_QueryPoolImpl::endQuery(VkCommandBuffer commandBuffer)
{
    vkCmdEndQuery(commandBuffer, queryPool, 0);
}

void VK_QueryPoolImpl::setQueryCallback(std::function<void (const std::vector<uint64_t> &)> fn)
{
    if (fn)
        queryCallback = fn;
}

void VK_QueryPoolImpl::query()
{
    queryData.assign(queryData.size(), 0);

    vkGetQueryPoolResults(
        context->getDevice(),
        queryPool,
        0, 1, sizeof(uint64_t) * queryData.size(), queryData.data(), sizeof(uint64_t),
        VK_QUERY_RESULT_64_BIT
    );

    if (queryCallback)
        queryCallback(queryData);
}
