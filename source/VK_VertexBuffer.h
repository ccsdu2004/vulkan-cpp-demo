#ifndef VK_VERTEXBUFFER_H
#define VK_VERTEXBUFFER_H
#include <cstring>
#include <iostream>
#include "VK_Context.h"
#include "VK_Buffer.h"

class VK_VertexBuffer : public VK_Buffer
{
public:
    VK_VertexBuffer(VK_Context* context, VkDevice vkDevice);
    virtual ~VK_VertexBuffer();
public:
    void release()override;

    void create(const std::vector<float>& vertices, int32_t count, const std::vector<uint16_t>& indices);
    void create(const std::vector<VK_Vertex>& vertices, const std::vector<uint16_t>& indices);
    void render(VkCommandBuffer command)override;
private:
    template<class T>
    inline void createBufferData(const std::vector<T>& input, VkBuffer& inputBuffer, VkDeviceMemory& deviceMemory, bool vertex)
    {
        auto bufferSize = sizeof(T) * input.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        context->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, input.data(), (size_t) bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        context->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | (vertex ? VK_BUFFER_USAGE_VERTEX_BUFFER_BIT : VK_BUFFER_USAGE_INDEX_BUFFER_BIT), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, inputBuffer, deviceMemory);
        context->copyBuffer(stagingBuffer, inputBuffer, bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }
private:
    int count = 0;
    int indexedVertex = false;
    VkBuffer indexedBuffer = 0;
    VkDeviceMemory indexedBufferMemory = 0;
};

#endif // VK_VERTEXBUFFER_H
