#include "VK_UniformBufferImpl.h"
#include <chrono>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>

VK_UniformBufferImpl::VK_UniformBufferImpl(VK_Context *vkContext, VkDevice vkDevice, uint32_t uboSize):
    context(vkContext),
    device(vkDevice),
    bufferSize(uboSize)
{
    bufferData.resize(bufferSize);
}

VK_UniformBufferImpl::~VK_UniformBufferImpl()
{
}

void VK_UniformBufferImpl::initBuffer(VkDeviceSize swapImageChainSize)
{
    clearBuffer();
    uniformBuffers.resize(swapImageChainSize);
    uniformBuffersMemory.resize(swapImageChainSize);

    for (size_t i = 0; i < swapImageChainSize; i++) {
        context->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
    }

    needClear = true;
}

void VK_UniformBufferImpl::initDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout, VkDeviceSize swapImageChainSize, VkDescriptorPool pool)
{
    std::vector<VkDescriptorSetLayout> layouts(swapImageChainSize, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = swapImageChainSize;
    allocInfo.pSetLayouts = layouts.data();
    allocInfo.pNext = nullptr;

    descriptorSets.resize(swapImageChainSize);
    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        std::cout << "failed to allocate descriptor sets!" << std::endl;
    }

    for (size_t i = 0; i < swapImageChainSize; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = bufferSize;

        std::array<VkWriteDescriptorSet, 1> descriptorWrites;
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;
        descriptorWrites[0].pNext = nullptr;

        vkUpdateDescriptorSets(device, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }
}

void VK_UniformBufferImpl::release()
{
    clearBuffer();
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

void VK_UniformBufferImpl::bindDescriptorSets(uint32_t index, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[index], 0, nullptr);
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
