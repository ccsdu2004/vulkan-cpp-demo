#include <VK_DynamicStateImpl.h>

void VK_DynamicStateImpl::apply(VkCommandBuffer buffer)
{
    if(viewport.has_value())
        vkCmdSetViewport(buffer, 0, 1, &viewport.value());

    if(scissor.has_value())
        vkCmdSetScissor(buffer, 0, 1, &scissor.value());

    if(lineWidth.has_value())
        vkCmdSetLineWidth(buffer, lineWidth.value());

    if(depthBias.has_value()) {
        auto value = depthBias.value();
        vkCmdSetDepthBias(buffer, value[0], value[1], value[2]);
    }

    if(blendConstant.has_value()) {
        auto value = blendConstant.value();
        vkCmdSetBlendConstants(buffer, &value[0]);
    }

    if(depthBounds.has_value()) {
        auto value = depthBounds.value();
        vkCmdSetDepthBounds(buffer, value[0], value[1]);
    }

    if(stencilCompareMask.has_value()) {
        auto value = stencilCompareMask.value();
        vkCmdSetStencilCompareMask(buffer, std::get<0>(value), std::get<1>(value));
    }

    if(stencilWriteMask.has_value()) {
        auto value = stencilWriteMask.value();
        vkCmdSetStencilWriteMask(buffer, std::get<0>(value), std::get<1>(value));
    }

    if(stencilReferenceMask.has_value()) {
        auto value = stencilReferenceMask.value();
        vkCmdSetStencilReference(buffer, std::get<0>(value), std::get<1>(value));
    }
}

void VK_DynamicStateImpl::release()
{
    delete this;
}

void VK_DynamicStateImpl::applyDynamicViewport(const VkViewport &inputViewport)
{
    viewport = inputViewport;
}

void VK_DynamicStateImpl::applyDynamicScissor(const VkRect2D &inputScissor)
{
    scissor = inputScissor;
}

void VK_DynamicStateImpl::applyDynamicLineWidth(float inputLineWidth)
{
    lineWidth = inputLineWidth;
}

void VK_DynamicStateImpl::applyDynamicDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
    depthBias = glm::vec3(depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

void VK_DynamicStateImpl::applyDynamicBlendConstants(const float inputBlendConstants[4])
{
    blendConstant = glm::vec4(inputBlendConstants[0], inputBlendConstants[2], inputBlendConstants[3], inputBlendConstants[3]);
}

void VK_DynamicStateImpl::applyDynamicDepthBounds(float minDepthBounds, float maxDepthBounds)
{
    depthBounds = glm::vec2(minDepthBounds, maxDepthBounds);
}

void VK_DynamicStateImpl::applyDynamicStencilCompareMask(VkStencilFaceFlags faceMask, uint32_t compareMask)
{
    stencilCompareMask = std::make_tuple(faceMask, compareMask);
}

void VK_DynamicStateImpl::applyDynamicStencilWriteMask(VkStencilFaceFlags faceMask, uint32_t writeMask)
{
    stencilWriteMask = std::make_tuple(faceMask, writeMask);
}

void VK_DynamicStateImpl::applyDynamicStencilReference(VkStencilFaceFlags faceMask, uint32_t reference)
{
    stencilReferenceMask = std::make_tuple(faceMask, reference);
}
