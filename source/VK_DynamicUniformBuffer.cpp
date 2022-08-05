#include <cstring>
#include <chrono>
#include <glm/gtx/transform.hpp>
#include <VK_DynamicUniformBuffer.h>
#include <iostream>

VK_DynamicUniformBuffer::VK_DynamicUniformBuffer(VK_ContextImpl *vkContext, uint32_t binding,
                                                 uint32_t uboSize, uint32_t count):
    context(vkContext),
    bindingId(binding),
    uniformCount(count)
{
    auto property = context->getPhysicalDeviceProperties();
    if (property.limits.minUniformBufferOffsetAlignment)
        uboSize = (uboSize + property.limits.minUniformBufferOffsetAlignment - 1) &
                  ~(property.limits.minUniformBufferOffsetAlignment - 1);

    bufferSize = uboSize;
    bufferData.resize(uboSize * uniformCount);
}

VK_DynamicUniformBuffer::~VK_DynamicUniformBuffer()
{
}

bool VK_DynamicUniformBuffer::isDynamicBuffer() const
{
    return true;
}

uint32_t VK_DynamicUniformBuffer::getBufferSize() const
{
    return bufferSize;
}

uint32_t VK_DynamicUniformBuffer::getBufferCount() const
{
    return uniformCount;
}

void VK_DynamicUniformBuffer::initBuffer(uint32_t swapImageChainSize)
{
    clearBuffer();

    uniformBuffers.resize(swapImageChainSize);
    uniformBuffersMemory.resize(swapImageChainSize);
    bufferInfos.resize(swapImageChainSize);

    for (size_t i = 0; i < swapImageChainSize; i++) {
        context->createBuffer(uniformCount * bufferSize,
                              VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i],
                              uniformBuffersMemory[i]);
        bufferInfos[i].buffer = uniformBuffers[i];
        bufferInfos[i].offset = 0;
        bufferInfos[i].range = bufferSize;
    }

    needClear = true;
}

VkWriteDescriptorSet VK_DynamicUniformBuffer::createWriteDescriptorSet(uint32_t index,
                                                                       VkDescriptorSet descriptorSet) const
{
    assert(index < bufferInfos.size());
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = bindingId;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType =
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfos[index];

    descriptorWrite.pTexelBufferView = nullptr;
    return descriptorWrite;
}

void VK_DynamicUniformBuffer::release()
{
    clearBuffer();
    uniformBuffers.clear();
    uniformBuffersMemory.clear();
    delete this;
}

void VK_DynamicUniformBuffer::setWriteDataCallback(std::function<uint32_t (char *&, uint32_t)> cb)
{
    if (cb)
        writeDataCallback = cb;
}

void VK_DynamicUniformBuffer::update(uint32_t index)
{
    if (!writeDataCallback) {
        std::cerr << "please set write data callback function" << std::endl;
        return;
    }

    char *userData = bufferData.data();
    uint32_t size = writeDataCallback(userData, bufferData.size());
    if (size != bufferData.size()) {
        std::cerr << "write data callback size error" << std::endl;
        return;
    }

    void *gpuData = nullptr;
    vkMapMemory(context->getDevice(), uniformBuffersMemory[index], 0, bufferSize * uniformCount, 0,
                &gpuData);
    memcpy(gpuData, userData, bufferSize * uniformCount);
    vkUnmapMemory(context->getDevice(), uniformBuffersMemory[index]);
}

void VK_DynamicUniformBuffer::clearBuffer()
{
    for (size_t i = 0; i < uniformBuffers.size(); i++) {
        vkDestroyBuffer(context->getDevice(), uniformBuffers[i], context->getAllocation());
        uniformBuffers[i] = VK_NULL_HANDLE;
    }
    for (size_t i = 0; i < uniformBuffers.size(); i++) {
        vkFreeMemory(context->getDevice(), uniformBuffersMemory[i], context->getAllocation());
        uniformBuffersMemory[i] = nullptr;
    }
}
