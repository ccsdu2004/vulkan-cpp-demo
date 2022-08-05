#include "VK_DescriptorSets.h"
#include "VK_ContextImpl.h"
#include "VK_UniformBuffer.h"
#include <iostream>

VK_DescriptorSets::VK_DescriptorSets(VK_ContextImpl *vkContext):
    context(vkContext)
{
}

VK_DescriptorSets::~VK_DescriptorSets()
{
}

void VK_DescriptorSets::init(VkDescriptorPool pool, VkDescriptorSetLayout setLayout)
{
    std::vector<VkDescriptorSetLayout> layouts(context->getSwapImageCount(), setLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = context->getSwapImageCount();
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(context->getSwapImageCount());
    dynamicOffsets.resize(context->getSwapImageCount());

    if (vkAllocateDescriptorSets(context->getDevice(), &allocInfo,
                                 descriptorSets.data()) != VK_SUCCESS) {
        std::cerr << "failed to allocate descriptor sets" << std::endl;
        return;
    }
}

void VK_DescriptorSets::update(const std::list<VK_UniformBuffer *> &uniformBuffers,
                               const std::map<VK_ImageView *, uint32_t> &imageViews)
{
    for (size_t i = 0; i < context->getSwapImageCount(); i++) {
        std::vector<uint32_t> offsets;

        std::vector<VkWriteDescriptorSet> descriptorWrites;
        for (auto buffer : uniformBuffers) {
            if (buffer->isDynamicBuffer() && buffer->getBufferCount() > 1) {
                for (size_t k = 0; k < buffer->getBufferCount(); k++)
                    offsets.push_back(buffer->getBufferSize());
            }
            descriptorWrites.push_back(buffer->createWriteDescriptorSet(i, descriptorSets[i]));
        }

        dynamicOffsets.at(i) = offsets;

        for (auto imageView : imageViews) {
            descriptorWrites.push_back(imageView.first->createWriteDescriptorSet(descriptorSets[i],
                                                                                 imageView.second));
        }

        vkUpdateDescriptorSets(context->getDevice(), descriptorWrites.size(), &descriptorWrites[0], 0,
                               nullptr);
    }
}

void VK_DescriptorSets::bind(VkCommandBuffer command, VkPipelineLayout pipelineLayout,
                             uint32_t index)
{
    vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                            &descriptorSets[index], 0,
                            nullptr);
}

void VK_DescriptorSets::render(VkCommandBuffer command, VkPipelineLayout pipelineLayout,
                               uint32_t index, const std::list<VK_Buffer *> &buffers)
{
    auto offsets = dynamicOffsets[index];
    if (offsets.size() == 0) {
        vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                                &descriptorSets[index], 0, nullptr);

        for (auto buffer : buffers)
            buffer->render(command);
        return;
    }

    size_t count = offsets.size();
    for (uint32_t i = 0; i < count; i++) {
        uint32_t offset[1] = {i *(uint32_t)offsets[i]};
        vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                                &descriptorSets[index], 1,
                                offset);

        for (auto buffer : buffers)
            buffer->render(command);
    }
}

