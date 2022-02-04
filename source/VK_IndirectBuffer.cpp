#include "VK_IndirectBuffer.h"

void VK_IndirectBuffer::create(uint32_t instanceCount,
                               uint32_t oneInstanceSize, uint32_t vertexCount)
{
    for (uint32_t i = 0; i < instanceCount; i++) {
        VkDrawIndirectCommand indirectCmd{};
        indirectCmd.instanceCount = instanceCount;
        indirectCmd.firstInstance = i * instanceCount;
        indirectCmd.firstVertex = i * oneInstanceSize;
        indirectCmd.vertexCount = vertexCount;
        indirectCommands.push_back(indirectCmd);
    }

    createBufferData<VkDrawIndirectCommand>(indirectCommands, buffer, bufferMemory);
    count = instanceCount;
}

void VK_IndirectBuffer::render(VkCommandBuffer command)
{
    vkCmdDrawIndirect(command, buffer, 0, indirectCommands.size(), sizeof(VkDrawIndirectCommand));
}
