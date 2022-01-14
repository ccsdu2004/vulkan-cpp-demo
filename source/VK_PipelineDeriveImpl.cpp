#include <iostream>
#include <cassert>
#include "VK_PipelineDeriveImpl.h"
#include "VK_ContextImpl.h"

VK_PipelineDeriveImpl::VK_PipelineDeriveImpl(VK_ContextImpl *context, VK_ShaderSet *shader, VK_PipelineImpl *inputParent):
    VK_PipelineImpl(context, shader, inputParent)
{
}

bool VK_PipelineDeriveImpl::create()
{
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    {
        VK_ShaderSet* shader = shaderSet;

        if(!shader) {
            assert(parent);
            shader = parent->getShaderSet();
        }

        pipelineCreateInfo.stageCount = shader->getCreateInfoCount();
        pipelineCreateInfo.pStages = shader->getCreateInfoData();
    }

    auto vertexInputStateCreateInfo = getVertexInputStateCreateInfo();
    pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;

    auto inputAssemblyStateCreateInfo = getInputAssemblyStateCreateInfo();
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    auto viewports = getViewports();
    viewportState.viewportCount = viewports.getViewportCount();
    viewportState.pViewports = viewports.getViewportData();
    viewportState.scissorCount = viewports.getViewportCount();
    viewportState.pScissors = viewports.getScissorData();
    pipelineCreateInfo.pViewportState = &viewportState;

    auto rasterizationStateCreateInfo = getRasterizationStateCreateInfo();
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;

    auto depthStencilStateCreateInfo = getDepthStencilStateCreateInfo();
    pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;

    auto multiSampleStateCreateInfo = getMultisampleStateCreateInfo();
    pipelineCreateInfo.pMultisampleState = &multiSampleStateCreateInfo;

    auto colorBlendStateCreateInfo = getColorBlendStateCreateInfo();
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;

    if (!tessellationStateCreateInfo.has_value())
        pipelineCreateInfo.pTessellationState = nullptr;
    else
        pipelineCreateInfo.pTessellationState = &tessellationStateCreateInfo.value();

    auto dynamicState = getDynamicState()->createDynamicStateCreateInfo(0);
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.layout = context->getPipelineLayout();
    pipelineCreateInfo.renderPass = context->getRenderPass();
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.pNext = nullptr;

    pipelineCreateInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
    pipelineCreateInfo.basePipelineHandle = parent->pipeline;
    pipelineCreateInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(context->getDevice(), context->getPipelineCache()->getPipelineCache(), 1, &pipelineCreateInfo, context->getAllocation(),
                                  &pipeline) != VK_SUCCESS) {
        std::cerr << "failed to create drivative graphics pipeline!" << std::endl;
        return false;
    }

    return true;
}
