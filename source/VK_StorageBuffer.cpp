#include <cstring>
#include <chrono>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <VK_StorageBuffer.h>

VK_StorageBuffer::VK_StorageBuffer(VK_ContextImpl *vkContext, uint32_t binding,
                                   uint32_t uboSize):
    context(vkContext),
    bindingId(binding),
    bufferSize(uboSize)
{
    bufferData.resize(bufferSize);
}

VK_StorageBuffer::~VK_StorageBuffer()
{
}

bool VK_StorageBuffer::isDynamicBuffer() const
{
    return false;
}

uint32_t VK_StorageBuffer::getBufferSize() const
{
    return bufferSize;
}

uint32_t VK_StorageBuffer::getBufferCount() const
{
    return 1;
}

void VK_StorageBuffer::initBuffer(uint32_t swapImageChainSize)
{
    clearBuffer();

    uniformBuffers.resize(swapImageChainSize);
    uniformBuffersMemory.resize(swapImageChainSize);
    bufferInfos.resize(swapImageChainSize);

    for (size_t i = 0; i < swapImageChainSize; i++) {
        context->createBuffer(bufferSize,
                              VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i],
                              uniformBuffersMemory[i]);
        bufferInfos[i].buffer = uniformBuffers[i];
        bufferInfos[i].offset = 0;
        bufferInfos[i].range = bufferSize;
    }

    needClear = true;
}

VkWriteDescriptorSet VK_StorageBuffer::createWriteDescriptorSet(uint32_t index,
                                                                VkDescriptorSet descriptorSet) const
{
    assert(index < bufferInfos.size());
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = bindingId;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType =
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfos[index];
    return descriptorWrite;
}

void VK_StorageBuffer::release()
{
    clearBuffer();
    uniformBuffers.clear();
    uniformBuffersMemory.clear();
    delete this;
}

void VK_StorageBuffer::setWriteDataCallback(std::function<uint32_t (char *&, uint32_t)> cb)
{
    if (cb)
        writeDataCallback = cb;
}

void VK_StorageBuffer::update(uint32_t index)
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
    vkMapMemory(context->getDevice(), uniformBuffersMemory[index], 0, bufferSize, 0, &gpuData);
    memcpy(gpuData, userData, bufferSize);
    vkUnmapMemory(context->getDevice(), uniformBuffersMemory[index]);
}

void VK_StorageBuffer::clearBuffer()
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
