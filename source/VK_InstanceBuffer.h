#ifndef VK_INSTANCE_BUFFER_H
#define VK_INSTANCE_BUFFER_H
#include <VK_VertexBuffer.h>

struct InstanceData {
    glm::vec3 position;
    glm::vec3 rotate;
    float scale = 1.0f;
    uint32_t index = 0;
};

class VK_InstanceBuffer : public VK_Buffer
{
public:
    VK_InstanceBuffer() = delete;
    VK_InstanceBuffer(VK_Context *context);
    virtual ~VK_InstanceBuffer();
public:
    void create(uint32_t count, uint32_t itemSize, const char *data, uint32_t bind);
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
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, inputBuffer, deviceMemory);
        context->copyBuffer(stagingBuffer, inputBuffer, bufferSize);

        vkDestroyBuffer(context->getDevice(), stagingBuffer, getContext()->getAllocation());
        vkFreeMemory(context->getDevice(), stagingBufferMemory, getContext()->getAllocation());
    }
private:
    uint32_t bindID = 0;
    size_t count = 0;
};

#endif // VK_INDIRECTVERTEXBUFFER_H
