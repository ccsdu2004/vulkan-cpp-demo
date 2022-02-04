#ifndef VK_INDIRECTBUFFER_H
#define VK_INDIRECTBUFFER_H
#include <string.h>
#include <VK_Buffer.h>
#include <VK_Context.h>

class VK_IndirectBuffer : public VK_Buffer
{
public:
    VK_IndirectBuffer(VK_Context *vkContext):
        VK_Buffer(vkContext)
    {
    }

    ~VK_IndirectBuffer()
    {
    }
public:
    void create(uint32_t instanceCount, uint32_t oneInstanceSize, uint32_t vertexCount);
    size_t getDataCount() const override
    {
        return count;
    }

    void render(VkCommandBuffer command)override;
private:
    template<class T>
    inline void createBufferData(const std::vector<T> &input, VkBuffer &inputBuffer,
                                 VkDeviceMemory &deviceMemory)
    {
        auto bufferSize = sizeof(T) * input.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        context->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                              stagingBufferMemory);

        void *data;
        vkMapMemory(context->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, input.data(), (size_t) bufferSize);
        vkUnmapMemory(context->getDevice(), stagingBufferMemory);

        context->createBuffer(bufferSize,
                              VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, inputBuffer, deviceMemory);
        context->copyBuffer(stagingBuffer, inputBuffer, bufferSize);

        vkDestroyBuffer(context->getDevice(), stagingBuffer, getContext()->getAllocation());
        vkFreeMemory(context->getDevice(), stagingBufferMemory, getContext()->getAllocation());
    }
private:
    std::vector<VkDrawIndirectCommand> indirectCommands;
    size_t count = 0;
};

#endif // VK_INDIRECTBUFFER_H
