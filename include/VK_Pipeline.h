#ifndef VK_PIPLELINE_H
#define VK_PIPLELINE_H
#include <optional>
#include <vulkan/vulkan.h>
#include <VK_Deleter.h>

class VK_ShaderSet;
class VK_Viewports;
class VK_Buffer;
class VK_DynamicState;

class VK_Pipeline : public VK_Deleter
{
public:
    virtual void setVertexInputStateCreateInfo(const VkPipelineVertexInputStateCreateInfo& createInfo) = 0;
    virtual VkPipelineVertexInputStateCreateInfo getVertexInputStateCreateInfo()const = 0;

    virtual void setInputAssemblyStateCreateInfo(const VkPipelineInputAssemblyStateCreateInfo &createInfo) = 0;
    virtual VkPipelineInputAssemblyStateCreateInfo getInputAssemblyStateCreateInfo()const = 0;

    virtual void setRasterizationStateCreateInfo(const VkPipelineRasterizationStateCreateInfo &createInfo) = 0;
    virtual VkPipelineRasterizationStateCreateInfo getRasterizationStateCreateInfo()const = 0;

    virtual VkPipelineDepthStencilStateCreateInfo getDepthStencilStateCreateInfo()const = 0;
    virtual void setDepthStencilStateCreateInfo(const VkPipelineDepthStencilStateCreateInfo &createInfo) = 0;

    static VkPipelineTessellationStateCreateInfo createPipelineTessellationStateCreateInfo(uint32_t patch);
    virtual void setTessellationStateCreateInfo(const VkPipelineTessellationStateCreateInfo &createInfo) = 0;
    virtual VkPipelineTessellationStateCreateInfo getTessellationStateCreateInfo() = 0;

    virtual void setViewports(const VK_Viewports& viewports) = 0;
    virtual VK_Viewports getViewports()const = 0;

    virtual void setMultisampleStateCreateInfo(const VkPipelineMultisampleStateCreateInfo& createInfo) = 0;
    virtual VkPipelineMultisampleStateCreateInfo getMultisampleStateCreateInfo()const = 0;

    virtual void setColorBlendStateCreateInfo(const VkPipelineColorBlendStateCreateInfo& createInfo) = 0;
    virtual VkPipelineColorBlendStateCreateInfo getColorBlendStateCreateInfo()const = 0;

    virtual VK_DynamicState* getDynamicState()const = 0;
public:
    virtual bool create() = 0;
    virtual void addRenderBuffer(VK_Buffer* buffer) = 0;
    virtual VK_Pipeline* fork(VK_ShaderSet* shaderSet = nullptr) = 0;
    virtual bool needRecreate()const = 0;
};

#endif // VK_PIPLELINE_H
