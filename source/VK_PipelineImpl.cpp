#include <iostream>
#include <VK_PipelineImpl.h>
#include <VK_ContextImpl.h>
#include <VK_PipelineDeriveImpl.h>

VK_PipelineImpl::VK_PipelineImpl(VK_ContextImpl *inputContext, VK_ShaderSet *shader,
                                 VK_PipelineImpl *inputParent):
    context(inputContext),
    shaderSet(shader),
    parent(inputParent)
{
    if (!shaderSet)
        shaderSet = parent->getShaderSet();

    vkDynamicState = new VK_DynamicStateImpl(this);

    descriptorSets = std::make_shared<VK_DescriptorSets>(inputContext);
    pipelineLayout = std::make_shared<VK_PipelineLayout>(inputContext);
}

VK_PipelineImpl::~VK_PipelineImpl()
{
    vkDynamicState->release();
}

VK_ShaderSet *VK_PipelineImpl::getShaderSet() const
{
    return shaderSet;
}

void VK_PipelineImpl::setVertexInputStateCreateInfo(const VkPipelineVertexInputStateCreateInfo
                                                    &createInfo)
{
    vertexInputStateCreateInfo = createInfo;
    needUpdate = true;
}

VkPipelineVertexInputStateCreateInfo VK_PipelineImpl::getVertexInputStateCreateInfo() const
{
    if (vertexInputStateCreateInfo.has_value())
        return vertexInputStateCreateInfo.value();
    assert(parent);
    return parent->getVertexInputStateCreateInfo();
}

void VK_PipelineImpl::setInputAssemblyStateCreateInfo(const VkPipelineInputAssemblyStateCreateInfo
                                                      &createInfo)
{
    inputAssemblyStateCreateInfo = createInfo;
    needUpdate = true;
}

VkPipelineInputAssemblyStateCreateInfo VK_PipelineImpl::getInputAssemblyStateCreateInfo() const
{
    if (inputAssemblyStateCreateInfo.has_value())
        return inputAssemblyStateCreateInfo.value();
    assert(parent);
    return parent->getInputAssemblyStateCreateInfo();
}

void VK_PipelineImpl::setRasterizationStateCreateInfo(const VkPipelineRasterizationStateCreateInfo
                                                      &createInfo)
{
    rasterizationStateCreateInfo = createInfo;
    needUpdate = true;
}

VkPipelineRasterizationStateCreateInfo VK_PipelineImpl::getRasterizationStateCreateInfo() const
{
    if (rasterizationStateCreateInfo.has_value())
        return rasterizationStateCreateInfo.value();
    assert(parent);
    return parent->getRasterizationStateCreateInfo();
}

void VK_PipelineImpl::setDepthStencilStateCreateInfo(const VkPipelineDepthStencilStateCreateInfo
                                                     &createInfo)
{
    depthStencilStateCreateInfo = createInfo;
    needUpdate = true;
}

VkPipelineDepthStencilStateCreateInfo VK_PipelineImpl::getDepthStencilStateCreateInfo() const
{
    if (depthStencilStateCreateInfo.has_value())
        return depthStencilStateCreateInfo.value();
    assert(parent);
    return parent->getDepthStencilStateCreateInfo();
}

void VK_PipelineImpl::setTessellationStateCreateInfo(const VkPipelineTessellationStateCreateInfo
                                                     &createInfo)
{
    tessellationStateCreateInfo = createInfo;
    needUpdate = true;
}

VkPipelineTessellationStateCreateInfo VK_PipelineImpl::getTessellationStateCreateInfo()
{
    if (tessellationStateCreateInfo.has_value())
        return tessellationStateCreateInfo.value();
    assert(parent);
    return parent->getTessellationStateCreateInfo();
}

void VK_PipelineImpl::setMultisampleStateCreateInfo(const VkPipelineMultisampleStateCreateInfo
                                                    &createInfo)
{
    multiSampleStateCreateInfo = createInfo;
    needUpdate = true;
}

VkPipelineMultisampleStateCreateInfo VK_PipelineImpl::getMultisampleStateCreateInfo() const
{
    if (multiSampleStateCreateInfo.has_value())
        return multiSampleStateCreateInfo.value();
    assert(parent);
    return parent->getMultisampleStateCreateInfo();
}

void VK_PipelineImpl::setColorBlendStateCreateInfo(const VkPipelineColorBlendStateCreateInfo
                                                   &createInfo)
{
    colorBlendStateCreateInfo = createInfo;
    needUpdate = true;
}

VkPipelineColorBlendStateCreateInfo VK_PipelineImpl::getColorBlendStateCreateInfo() const
{
    if (colorBlendStateCreateInfo.has_value()) {
        return colorBlendStateCreateInfo.value();
    }
    assert(parent);

    return parent->getColorBlendStateCreateInfo();
}

VK_DynamicState *VK_PipelineImpl::getDynamicState() const
{
    return vkDynamicState;
}

bool VK_PipelineImpl::create()
{
    return createPipeline(VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT);
}

void VK_PipelineImpl::addRenderBuffer(VK_Buffer *buffer)
{
    if (buffer)
        buffers.push_back(buffer);
}

VK_Pipeline *VK_PipelineImpl::fork(VK_ShaderSet *shaderSet)
{
    if (parent) {
        std::cerr << "VK_Pipeline drive failed" << std::endl;
        return nullptr;
    }

    if (!shaderSet)
        shaderSet = getShaderSet();
    auto child = new VK_PipelineDeriveImpl(context, shaderSet, this);
    context->addPipeline(child);
    return child;
}

bool VK_PipelineImpl::needRecreate() const
{
    return needUpdate;
}

void VK_PipelineImpl::setNeedRecreate()
{
    needUpdate = true;
}

void VK_PipelineImpl::render(VkCommandBuffer buffer, uint32_t index)
{
    pipelineLayout->pushConst(buffer);

    vkDynamicState->apply(buffer);
    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    if (pushDescriptor)
        pushDescriptor->push(buffer, pipelineLayout->getPipelineLayout());

    descriptorSets->render(buffer, pipelineLayout->getPipelineLayout(), index, buffers);
}

void VK_PipelineImpl::render(VkCommandBuffer buffer, uint32_t index,
                             std::shared_ptr<VK_SecondaryCommandBufferCallback> caller, uint32_t current, uint32_t total)
{
    pipelineLayout->pushConst(buffer);

    vkDynamicState->apply(buffer);
    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    if (pushDescriptor)
        pushDescriptor->push(buffer, pipelineLayout->getPipelineLayout());

    if (caller)
        caller->execute(context, buffer, current, total);

    descriptorSets->render(buffer, pipelineLayout->getPipelineLayout(), index, buffers);
}

void VK_PipelineImpl::release()
{
    vkDestroyPipeline(context->getDevice(), pipeline, context->getAllocation());
    pipelineLayout->release();
    descriptorSetLayout->release();
    descriptorPool->release();
}

void VK_PipelineImpl::prepare()
{
    initVertexInputStateCreateInfo(shaderSet);
    initMultisampleStateCreateInfo(context->getSampleCountFlagBits());
    initColorBlendStateCreateInfo();
    initInputAssemblyStateCreateInfo();
    initRasterizationStateCreateInfo();
    initDepthStencilStateCreateInfo();
    initColorBlendAttachmentState();
}

void VK_PipelineImpl::addPushConstant(const VkPushConstantRange &constantRange, const char *data)
{
    pipelineLayout->addPushConstant(constantRange, data);
}

void VK_PipelineImpl::addPushDescriptor(const VkWriteDescriptorSet &descriptor)
{
    if (!pushDescriptor)
        pushDescriptor = std::make_shared<VK_PushDescriptor>(context);

    pushDescriptor->addDescriptor(descriptor);
}

void VK_PipelineImpl::initVertexInputStateCreateInfo(VK_ShaderSet *shaderSet)
{
    VkPipelineVertexInputStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;

    createInfo.vertexBindingDescriptionCount = shaderSet->getVertexInputBindingDescriptionCount();
    createInfo.pVertexBindingDescriptions = shaderSet->getVertexInputBindingDescriptionData();

    createInfo.vertexAttributeDescriptionCount = shaderSet->getVertexAttributeDescriptionCount();
    createInfo.pVertexAttributeDescriptions = shaderSet->getVertexAttributeDescriptionData();

    setVertexInputStateCreateInfo(createInfo);
}

void VK_PipelineImpl::initMultisampleStateCreateInfo(VkSampleCountFlagBits sampleCount)
{
    VkPipelineMultisampleStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    createInfo.sampleShadingEnable = VK_FALSE;
    createInfo.rasterizationSamples = sampleCount;
    setMultisampleStateCreateInfo(createInfo);
}

void VK_PipelineImpl::initColorBlendStateCreateInfo()
{
    VkPipelineColorBlendStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    createInfo.logicOpEnable = VK_FALSE;
    createInfo.logicOp = VK_LOGIC_OP_COPY;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &colorBlendAttachment;
    createInfo.blendConstants[0] = 0.0f;
    createInfo.blendConstants[1] = 0.0f;
    createInfo.blendConstants[2] = 0.0f;
    createInfo.blendConstants[3] = 0.0f;
    setColorBlendStateCreateInfo(createInfo);
}

void VK_PipelineImpl::initInputAssemblyStateCreateInfo()
{
    VkPipelineInputAssemblyStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    createInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    createInfo.primitiveRestartEnable = VK_FALSE;
    setInputAssemblyStateCreateInfo(createInfo);
}

void VK_PipelineImpl::initRasterizationStateCreateInfo()
{
    VkPipelineRasterizationStateCreateInfo createInfo{};
    createInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    createInfo.depthClampEnable = VK_FALSE;
    createInfo.rasterizerDiscardEnable = VK_FALSE;
    createInfo.polygonMode = VK_POLYGON_MODE_FILL;
    createInfo.lineWidth = 1.0f;
    createInfo.cullMode = VK_CULL_MODE_NONE;
    createInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    createInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo = createInfo;
}

void VK_PipelineImpl::initColorBlendAttachmentState()
{
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT |
                                          VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
}

bool VK_PipelineImpl::createPipeline(VkPipelineCreateFlagBits flag)
{
    descriptorSetLayout = new VK_DescriptorSetLayout(context, shaderSet);
    pipelineLayout->create(descriptorSetLayout->getDescriptorSetLayout());

    shaderSet->updateDescriptorPoolSize(context->getSwapImageCount());
    descriptorPool = new VK_DescriptorPool(context);
    descriptorPool->create(shaderSet);

    descriptorSets->init(descriptorPool->getDescriptorPool(),
                         descriptorSetLayout->getDescriptorSetLayout());

    shaderSet->updateDescriptorSet(descriptorSets);

    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.flags = flag;

    {
        VK_ShaderSet *shader = shaderSet;

        if (!shader) {
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

    auto swapSize = context->getSwapChainExtent();
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapSize.width;
    viewport.height = (float) swapSize.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapSize;

    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    pipelineCreateInfo.pViewportState = &viewportState;

    auto rasterizationStateCreateInfo = getRasterizationStateCreateInfo();
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;

    auto depthStencilStateCreateInfo = getDepthStencilStateCreateInfo();
    pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;

    auto multiSampleStateCreateInfo = getMultisampleStateCreateInfo();
    pipelineCreateInfo.pMultisampleState = &multiSampleStateCreateInfo;

    VkPipelineColorBlendAttachmentState blend_attachment_state = {
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA,
        .alphaBlendOp = VK_BLEND_OP_MAX,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    auto colorBlendStateCreateInfo = getColorBlendStateCreateInfo();
    colorBlendStateCreateInfo.pAttachments = &blend_attachment_state;
    colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[2] = 1.0f;
    colorBlendStateCreateInfo.blendConstants[3] = 1.0f;
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;

    if (!tessellationStateCreateInfo.has_value())
        pipelineCreateInfo.pTessellationState = nullptr;
    else
        pipelineCreateInfo.pTessellationState = &tessellationStateCreateInfo.value();

    auto dynamicState = getDynamicState()->createDynamicStateCreateInfo(0);
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.layout = pipelineLayout->getPipelineLayout();
    pipelineCreateInfo.renderPass = context->getRenderPass();
    pipelineCreateInfo.subpass = 0;
    auto parentPipeline = parent ? parent->pipeline : nullptr;
    pipelineCreateInfo.basePipelineHandle = parentPipeline;
    pipelineCreateInfo.pNext = nullptr;
    pipelineCreateInfo.basePipelineIndex = parentPipeline == VK_NULL_HANDLE ? 0 : -1;

    if (vkCreateGraphicsPipelines(context->getDevice(), context->getPipelineCache()->getPipelineCache(),
                                  1, &pipelineCreateInfo, context->getAllocation(),
                                  &pipeline) != VK_SUCCESS) {
        std::cerr << "failed to create graphics pipeline!" << std::endl;
        return false;
    }

    needUpdate = false;
    return true;
}

void VK_PipelineImpl::initDepthStencilStateCreateInfo()
{
    VkPipelineDepthStencilStateCreateInfo createInfo{};
    createInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.depthTestEnable = VK_TRUE;
    createInfo.depthWriteEnable = VK_TRUE;
    createInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    createInfo.depthBoundsTestEnable = VK_FALSE;
    createInfo.minDepthBounds = 0.0f;
    createInfo.maxDepthBounds = 1.0f;
    createInfo.stencilTestEnable = VK_FALSE;
    createInfo.back.failOp = VK_STENCIL_OP_KEEP;
    createInfo.back.passOp = VK_STENCIL_OP_KEEP;
    createInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
    createInfo.back.compareMask = 0;
    createInfo.back.reference = 0;
    createInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
    createInfo.back.writeMask = 0;
    createInfo.front = createInfo.back;

    setDepthStencilStateCreateInfo(createInfo);
}

