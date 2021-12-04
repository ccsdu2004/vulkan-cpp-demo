#include "VK_UniformBufferImpl.h"
#include <cstring>
#include <chrono>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>

VK_UniformBufferImpl::VK_UniformBufferImpl(VK_ContextImpl *vkContext, VkDevice vkDevice, uint32_t binding, uint32_t uboSize):
    context(vkContext),
    device(vkDevice),
    bindingId(binding),
    bufferSize(uboSize)
{
    bufferData.resize(bufferSize);
}

VK_UniformBufferImpl::~VK_UniformBufferImpl()
{
}

void VK_UniformBufferImpl::initBuffer(uint32_t swapImageChainSize)
{
    clearBuffer();
    uniformBuffers.resize(swapImageChainSize);
    uniformBuffersMemory.resize(swapImageChainSize);
    bufferInfos.resize(swapImageChainSize);

    for (size_t i = 0; i < swapImageChainSize; i++) {
        context->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
        bufferInfos[i].buffer = uniformBuffers[i];
        bufferInfos[i].offset = 0;
        bufferInfos[i].range = bufferSize;
    }

    needClear = true;
}

VkWriteDescriptorSet VK_UniformBufferImpl::createWriteDescriptorSet(uint32_t index, VkDescriptorSet descriptorSet) const
{
    assert(index < bufferInfos.size());
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = bindingId;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfos[index];
    return descriptorWrite;
}

void VK_UniformBufferImpl::release()
{
    if(isRemoveFromContainerWhenRelease())
        context->removeUniformBuffer(this);
    uniformBuffers.clear();
    uniformBuffersMemory.clear();

    //vkFreeDescriptorSets(device, descriptorPool, descriptorSets.size(), &descriptorSets[0]);
    delete this;
}

void VK_UniformBufferImpl::setWriteDataCallback(std::function<uint32_t (char *&, uint32_t)> cb)
{
    if(cb)
        writeDataCallback = cb;
}

void VK_UniformBufferImpl::update(uint32_t index)
{
    if(!writeDataCallback) {
        std::cerr << "please set write data callback function" << std::endl;
    }

    char* userData = bufferData.data();
    uint32_t size = writeDataCallback(userData, bufferData.size());
    if(size != bufferData.size()) {
        std::cerr << "write data callback size error" << std::endl;
        return;
    }

    void* gpuData;
    vkMapMemory(device, uniformBuffersMemory[index], 0, bufferSize, 0, &gpuData);
    memcpy(gpuData, userData, bufferSize);
    vkUnmapMemory(device, uniformBuffersMemory[index]);
}

void VK_UniformBufferImpl::clearBuffer()
{
    for(size_t i = 0; i < uniformBuffers.size(); i++) {
        vkDestroyBuffer(device, uniformBuffers[i], nullptr);
        uniformBuffers[i] = VK_NULL_HANDLE;
    }
    for(size_t i = 0; i < uniformBuffers.size(); i++) {
        vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
        uniformBuffersMemory[i] = 0;
    }
}
