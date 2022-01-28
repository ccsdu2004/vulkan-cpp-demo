#ifndef VK_PIPELINELAYOUT_H
#define VK_PIPELINELAYOUT_H
#include <vector>
#include <vulkan/vulkan.h>
#include <VK_Deleter.h>

class VK_ContextImpl;

class VK_PipelineLayout : public VK_Deleter
{
public:
    VK_PipelineLayout(VK_ContextImpl *vkContext);
    ~VK_PipelineLayout();
public:
    void addPushConstant(const VkPushConstantRange &constantRange, const char *data);
    VkPipelineLayout getPipelineLayout()const
    {
        return pipelineLayout;
    }
    void create(VkDescriptorSetLayout setLayout);
    void pushConst(VkCommandBuffer commandBuffer);
    void release()override;
private:
    void destroy();
private:
    VK_ContextImpl *context = nullptr;

    std::vector<VkPushConstantRange> pushConstantRange;
    std::vector<const char *> pushConstantData;

    VkPipelineLayout pipelineLayout = nullptr;
};

#endif // VK_PIPELINELAYOUT_H
