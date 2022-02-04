#include <iostream>
#include <VK_InstanceBuffer.h>
#include <VK_Context.h>

VK_InstanceBuffer::VK_InstanceBuffer(VK_Context *vkContext):
    VK_Buffer(vkContext)
{
}

VK_InstanceBuffer::~VK_InstanceBuffer()
{
}

void VK_InstanceBuffer::create(uint32_t count, uint32_t itemSize, const char *data, uint32_t bind)
{
    std::vector<char> items;
    std::copy(data, data + count * itemSize, std::back_inserter(items));
    createBufferData(items, buffer, bufferMemory);
    bindID = bind;
    this->count = count;
}

void VK_InstanceBuffer::render(VkCommandBuffer command)
{
    VkBuffer vertexBuffers[] = {buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command, bindID, 1, vertexBuffers, offsets);
}
