#ifndef VK_PIPLELINEIMPL_H
#define VK_PIPLELINEIMPL_H
#include <list>
#include <memory>
#include <VK_Pipeline.h>
#include "VK_PushDescriptor.h"
#include "VK_DescriptorSets.h"
#include "VK_DescriptorSetLayout.h"
#include "VK_DescriptorPool.h"
#include "VK_PipelineLayout.h"
#include "VK_PushDescriptor.h"

class VK_ContextImpl;
class VK_DynamicStateImpl;

class VK_PipelineImpl : public VK_Pipeline
{
    friend class VK_PipelineDeriveImpl;
public:
    VK_PipelineImpl() = delete;
    VK_PipelineImpl(VK_ContextImpl *context, VK_ShaderSet *shaderSet,
                    VK_PipelineImpl *inputParent = nullptr);
    virtual ~VK_PipelineImpl();
public:
    VK_ShaderSet *getShaderSet()const;

    void setVertexInputStateCreateInfo(const VkPipelineVertexInputStateCreateInfo &createInfo)override;
    VkPipelineVertexInputStateCreateInfo getVertexInputStateCreateInfo()const override;

    void setInputAssemblyStateCreateInfo(const VkPipelineInputAssemblyStateCreateInfo &createInfo)
    override;
    VkPipelineInputAssemblyStateCreateInfo getInputAssemblyStateCreateInfo()const override;

    void setRasterizationStateCreateInfo(const VkPipelineRasterizationStateCreateInfo &createInfo)
    override;
    VkPipelineRasterizationStateCreateInfo getRasterizationStateCreateInfo()const override;

    VkPipelineDepthStencilStateCreateInfo getDepthStencilStateCreateInfo()const override;
    void setDepthStencilStateCreateInfo(const VkPipelineDepthStencilStateCreateInfo &createInfo)
    override;

    void setTessellationStateCreateInfo(const VkPipelineTessellationStateCreateInfo &createInfo)
    override;
    VkPipelineTessellationStateCreateInfo getTessellationStateCreateInfo()override;

    void setMultisampleStateCreateInfo(const VkPipelineMultisampleStateCreateInfo &createInfo)override;
    VkPipelineMultisampleStateCreateInfo getMultisampleStateCreateInfo()const override;

    void setColorBlendStateCreateInfo(const VkPipelineColorBlendStateCreateInfo &createInfo)override;
    VkPipelineColorBlendStateCreateInfo getColorBlendStateCreateInfo()const override;

    VK_DynamicState *getDynamicState()const override;
public:
    void prepare();
    void addPushConstant(const VkPushConstantRange &constantRange, const char *data) override;
    void addPushDescriptor(const VkWriteDescriptorSet& descriptor)override;
    virtual bool create() override;
    void addRenderBuffer(VK_Buffer *buffer)override;
    VK_Pipeline *fork(VK_ShaderSet *shaderSet) override;

    bool needRecreate()const override;
    void setNeedRecreate();

    void render(VkCommandBuffer buffer, uint32_t index);

    void release()override;
protected:
    void initVertexInputStateCreateInfo(VK_ShaderSet *shaderSet);
    void initMultisampleStateCreateInfo(VkSampleCountFlagBits sampleCount);
    void initColorBlendStateCreateInfo();
    void initInputAssemblyStateCreateInfo();
    void initRasterizationStateCreateInfo();
    void initDepthStencilStateCreateInfo();
    void initColorBlendAttachmentState();

    bool createPipeline(VkPipelineCreateFlagBits flag);
protected:
    VK_ContextImpl *context = nullptr;
    VK_ShaderSet *shaderSet = nullptr;
    VK_PipelineImpl *parent = nullptr;

    VK_DescriptorSetLayout *descriptorSetLayout = nullptr;
    std::shared_ptr<VK_PipelineLayout> pipelineLayout;
    VK_DescriptorPool *descriptorPool = nullptr;
    std::shared_ptr<VK_DescriptorSets> descriptorSets;

    VkPipeline pipeline = nullptr;
    std::shared_ptr<VK_PushDescriptor> pushDescriptor;

    std::optional<VkPipelineVertexInputStateCreateInfo> vertexInputStateCreateInfo;
    std::optional<VkPipelineInputAssemblyStateCreateInfo> inputAssemblyStateCreateInfo;
    std::optional<VkPipelineRasterizationStateCreateInfo> rasterizationStateCreateInfo;
    std::optional<VkPipelineDepthStencilStateCreateInfo> depthStencilStateCreateInfo;
    std::optional<VkPipelineMultisampleStateCreateInfo> multiSampleStateCreateInfo;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    std::optional<VkPipelineColorBlendStateCreateInfo> colorBlendStateCreateInfo;

    std::optional<VkPipelineTessellationStateCreateInfo> tessellationStateCreateInfo;
    VK_DynamicStateImpl *vkDynamicState = nullptr;

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    bool needUpdate = true;

    std::list<VK_Buffer *> buffers;
};

#endif // VK_PIPLELINEIMPL_H
