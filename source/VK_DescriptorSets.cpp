#include "VK_DescriptorSets.h"
#include "VK_ContextImpl.h"
#include "VK_UniformBuffer.h"

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
        std::vector<VkWriteDescriptorSet> descriptorWrites;
        for (auto buffer : uniformBuffers)
            descriptorWrites.push_back(buffer->createWriteDescriptorSet(i, descriptorSets[i]));

        for (auto imageView : imageViews) {
            descriptorWrites.push_back(imageView.first->createWriteDescriptorSet(descriptorSets[i], imageView.second));
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

