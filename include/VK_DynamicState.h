#ifndef VK_DYNAMICSTATE_H
#define VK_DYNAMICSTATE_H
#include <vulkan/vulkan.h>
#include <VK_Deleter.h>

class VK_DynamicState : public VK_Deleter
{
public:
    virtual void addDynamicState(VkDynamicState dynamicState) = 0;
    virtual VkPipelineDynamicStateCreateInfo createDynamicStateCreateInfo(VkPipelineDynamicStateCreateFlags flags) = 0;

    virtual void applyDynamicViewport(const VkViewport& viewport) = 0;
    virtual void applyDynamicScissor(const VkRect2D& scissor) = 0;
    virtual void applyDynamicLineWidth(float lineWidth) = 0;
    virtual void applyDynamicDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) = 0;
    virtual void applyDynamicBlendConstants(const float blendConstants[4]) = 0;
    virtual void applyDynamicDepthBounds(float minDepthBounds, float maxDepthBounds) = 0;
    virtual void applyDynamicStencilCompareMask(VkStencilFaceFlags faceMask, uint32_t compareMask) = 0;
    virtual void applyDynamicStencilWriteMask(VkStencilFaceFlags faceMask, uint32_t writeMask) = 0;
    virtual void applyDynamicStencilReference(VkStencilFaceFlags faceMask, uint32_t reference) = 0;
};

#endif // VK_DYNAMICSTATE_H
