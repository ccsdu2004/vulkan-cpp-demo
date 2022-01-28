#include <iostream>
#include <VK_PipelineLayout.h>
#include <VK_ContextImpl.h>

VK_PipelineLayout::VK_PipelineLayout(VK_ContextImpl *vkContext):
    context(vkContext)
{
}

VK_PipelineLayout::~VK_PipelineLayout()
{
}

void VK_PipelineLayout::create(VkDescriptorSetLayout setLayout)
{
    destroy();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pNext = nullptr;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &setLayout;
    pipelineLayoutInfo.pushConstantRangeCount = pushConstantRange.size();
    pipelineLayoutInfo.pPushConstantRanges = pushConstantRange.data();

    if (vkCreatePipelineLayout(context->getDevice(), &pipelineLayoutInfo, context->getAllocation(),
                               &pipelineLayout) != VK_SUCCESS) {
        std::cerr << "failed to create pipeline layout!" << std::endl;
    }
}

void VK_PipelineLayout::pushConst(VkCommandBuffer commandBuffer)
{
    size_t current = 0;
    for (auto pushConst : pushConstantRange) {
        vkCmdPushConstants(
            commandBuffer,
            pipelineLayout,
            pushConst.stageFlags,
            pushConst.offset,
            pushConst.size,
            pushConstantData[current]);
        current ++;
    }
}

void VK_PipelineLayout::addPushConstant(const VkPushConstantRange &constantRange, const char *data)
{
    if (data == nullptr)
        return;

    pushConstantRange.push_back(constantRange);
    pushConstantData.push_back(data);
}

void VK_PipelineLayout::release()
{
    destroy();
    //delete this;
}

void VK_PipelineLayout::destroy()
{
    if (pipelineLayout) {
        vkDestroyPipelineLayout(context->getDevice(), pipelineLayout, context->getAllocation());
        pipelineLayout = nullptr;
    }
}
