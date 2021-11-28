#include <iostream>
#include "VK_VertexBuffer.h"
#include "VK_Context.h"

VK_VertexBuffer::VK_VertexBuffer(VK_Context* vkContext, VkDevice vkDevice):
    VK_Buffer(vkContext, vkDevice)
{
}

VK_VertexBuffer::~VK_VertexBuffer()
{

}

void VK_VertexBuffer::release()
{
    if(device) {
        if(indexedBuffer)
            vkDestroyBuffer(device, indexedBuffer, nullptr);
        if(indexedBufferMemory)
            vkFreeMemory(device, indexedBufferMemory, nullptr);
    }

    VK_Buffer::release();
}

void VK_VertexBuffer::create(const std::vector<float> &vertices, int32_t size, const std::vector<uint16_t>& indices)
{
    createBufferData<float>(vertices, buffer, bufferMemory, true);
    count = vertices.size() / size;

    if(indices.empty()) {
        indexedVertex = false;
    } else {
        createBufferData<uint16_t>(indices, indexedBuffer, indexedBufferMemory, false);
        indexedVertex = true;
        count = indices.size();
    }
}

void VK_VertexBuffer::create(const std::vector<VK_Vertex> &vertices, const std::vector<uint16_t>& indices)
{
    createBufferData<VK_Vertex>(vertices, buffer, bufferMemory, true);
    count = vertices.size();

    if(indices.empty()) {
        indexedVertex = false;
    } else {
        createBufferData<uint16_t>(indices, indexedBuffer, indexedBufferMemory, false);
        indexedVertex = true;
        count = indices.size();
    }
}

void VK_VertexBuffer::render(VkCommandBuffer command)
{
    VkBuffer vertexBuffers[] = {buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command, 0, 1, vertexBuffers, offsets);
    if(!indexedVertex) {
        vkCmdDraw(command, count, 1, 0, 0);
    } else {
        vkCmdBindIndexBuffer(command, indexedBuffer, 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(command, static_cast<uint32_t>(count), 1, 0, 0, 0);
    }
}
