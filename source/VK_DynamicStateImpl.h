#ifndef VK_DYNAMICSTATEIMPL_H
#define VK_DYNAMICSTATEIMPL_H
#include <optional>
#include <tuple>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <VK_DynamicState.h>

class VK_DynamicStateImpl : public VK_DynamicState
{
public:
    void apply(VkCommandBuffer buffer);
    void release()override;
    void applyDynamicViewport(const VkViewport& viewport)override;
    void applyDynamicScissor(const VkRect2D& scissor)override;
    void applyDynamicLineWidth(float lineWidth)override;
    void applyDynamicDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)override;
    void applyDynamicBlendConstants(const float blendConstants[4])override;
    void applyDynamicDepthBounds(float minDepthBounds, float maxDepthBounds)override;
    void applyDynamicStencilCompareMask(VkStencilFaceFlags faceMask, uint32_t compareMask)override;
    void applyDynamicStencilWriteMask(VkStencilFaceFlags faceMask, uint32_t writeMask)override;
    void applyDynamicStencilReference(VkStencilFaceFlags faceMask, uint32_t reference)override;
private:
    std::optional<VkViewport> viewport;
    std::optional<VkRect2D> scissor;
    std::optional<float> lineWidth = 1.0f;
    std::optional<glm::vec3> depthBias;
    std::optional<glm::vec4> blendConstant;
    std::optional<glm::vec2> depthBounds;
    std::optional<std::tuple<VkStencilFaceFlags, uint32_t>> stencilCompareMask;
    std::optional<std::tuple<VkStencilFaceFlags, uint32_t>> stencilWriteMask;
    std::optional<std::tuple<VkStencilFaceFlags, uint32_t>> stencilReferenceMask;
};

#endif // VK_DYNAMICSTATE_H
