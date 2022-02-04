#include <iostream>
#include "VK_VertexBuffer.h"
#include "VK_Context.h"

VK_VertexBuffer::VK_VertexBuffer(VK_Context *vkContext):
    VK_Buffer(vkContext)
{
}

VK_VertexBuffer::~VK_VertexBuffer()
{

}

void VK_VertexBuffer::release()
{
    if (indexedBuffer)
        vkDestroyBuffer(context->getDevice(), indexedBuffer, getContext()->getAllocation());
    if (indexedBufferMemory)
        vkFreeMemory(context->getDevice(), indexedBufferMemory, getContext()->getAllocation());

    VK_Buffer::release();
}

void VK_VertexBuffer::create(const std::vector<float> &vertices, int32_t size,
                             const std::vector<uint32_t> &indices, bool indirect)
{
    createBufferData<float>(vertices, buffer, bufferMemory, true);
    count = vertices.size() / size;

    if (indices.empty()) {
        indexedVertex = false;
    } else {
        createBufferData<uint32_t>(indices, indexedBuffer, indexedBufferMemory, false);
        indexedVertex = true;
        count = indices.size();
    }

    indirectDraw = indirect;
}

void VK_VertexBuffer::create(const std::vector<VK_Vertex> &vertices,
                             const std::vector<uint32_t> &indices, bool indirect)
{
    createBufferData<VK_Vertex>(vertices, buffer, bufferMemory, true);
    count = vertices.size();

    if (indices.empty()) {
        indexedVertex = false;
    } else {
        createBufferData<uint32_t>(indices, indexedBuffer, indexedBufferMemory, false);
        indexedVertex = true;
        count = indices.size();
    }

    indirectDraw = indirect;
}

size_t VK_VertexBuffer::getDataCount() const
{
    return count;
}

void VK_VertexBuffer::render(VkCommandBuffer command)
{
    VkBuffer vertexBuffers[] = {buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command, 0, 1, vertexBuffers, offsets);
    if (!indexedVertex) {
        if(!indirectDraw)
            vkCmdDraw(command, count, 1, 0, 0);
    } else {
        vkCmdBindIndexBuffer(command, indexedBuffer, 0, VK_INDEX_TYPE_UINT32);
        if(!indirectDraw)
            vkCmdDrawIndexed(command, static_cast<uint32_t>(count), 1, 0, 0, 0);
    }
}
