#include "VK_UniformBuffer.h"
#include <vmath.h>
#include <algorithm>

VK_UniformBuffer::VK_UniformBuffer(VK_Context *vkContext, VkDevice vkDevice):
    context(vkContext),
    device(vkDevice)
{

}

VK_UniformBuffer::~VK_UniformBuffer()
{

}

void VK_UniformBuffer::release()
{
    context->setUniformBuffer(this);
    if(device) {
        for(auto itr = uniformBuffers.begin(); itr != uniformBuffers.end(); itr++) {
            vkDestroyBuffer(device, itr->first, nullptr);
            vkFreeMemory(device, itr->second, nullptr);
        }

        uniformBuffers.clear();
    }

    delete this;
}

bool VK_UniformBuffer::createBuffer(VkDeviceSize bufferSize, VkDeviceSize swapImageChainSize)
{
    if(bufferSize == 0 || swapImageChainSize == 0)
        return false;

    bufferData.resize(bufferSize);
    uniformBuffers.resize(swapImageChainSize);

    for (size_t i = 0; i < swapImageChainSize; i++) {
        context->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              uniformBuffers[i].first, uniformBuffers[i].second);
    }
    return true;
}

void VK_UniformBuffer::setWriteDataCallback(std::function<void (char *&, VkDeviceSize)> cb)
{
    if(cb)
        writeDataCallback = cb;
}

VkDeviceSize VK_UniformBuffer::getBufferSize() const
{
    return bufferData.size();
}

void VK_UniformBuffer::update(VkDeviceSize index)
{
    if(!writeDataCallback)
        return;

    char* ptr = &bufferData[0];
    writeDataCallback(ptr, bufferData.size());

    struct MVP {
        vmath::mat4 m;
        vmath::mat4 v;
        vmath::mat4 p;
    };

    MVP mvp;
    std::copy(&bufferData[0], &bufferData[0] + sizeof(MVP), reinterpret_cast<char*>(&mvp));

    for(int i = 0; i < 16; i++)
        std::cout << mvp.m[i / 4][i % 4] << " ";
    std::cout << std::endl;

    for(int i = 0; i < 16; i++)
        std::cout << mvp.v[i / 4][i % 4] << " ";
    std::cout << std::endl;

    for(int i = 0; i < 16; i++)
        std::cout << mvp.p[i / 4][i % 4] << " ";
    std::cout << std::endl;

    void* data;
    vkMapMemory(device, uniformBuffers[index].second, 0, bufferData.size(), 0, &data);
    memcpy(data, &bufferData[0], bufferData.size());
    vkUnmapMemory(device, uniformBuffers[index].second);
}
