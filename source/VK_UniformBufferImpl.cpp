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

void VK_UniformBufferImpl::cleanup()
{
    for(size_t i = 0; i < uniformBuffers.size(); i++)
        vkDestroyBuffer(device, uniformBuffers[i], nullptr);

    for(size_t i = 0; i < uniformBuffers.size(); i++)
        vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
}

void VK_UniformBufferImpl::release()
{
    cleanup();
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

void VK_UniformBufferImpl::update(uint32_t index, float aspect)
{
    /*static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    void* data;
    vkMapMemory(device, uniformBuffersMemory[index], 0, sizeof(model), 0, &data);
    memcpy(data, &model[0][0], sizeof(model));
    vkUnmapMemory(device, uniformBuffersMemory[index]);*/

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

void VK_UniformBufferImpl::initBuffer(VkDeviceSize swapImageChainSize)
{
    uniformBuffers.resize(swapImageChainSize);
    uniformBuffersMemory.resize(swapImageChainSize);

    for (size_t i = 0; i < swapImageChainSize; i++) {
        context->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
    }
}

void VK_UniformBufferImpl::initDescriptorSets(VkDescriptorSetLayout descriptorSetLayout, VkDeviceSize swapImageChainSize, VkDescriptorPool descriptorPool)
{
    std::vector<VkDescriptorSetLayout> layouts(swapImageChainSize, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
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

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }
}