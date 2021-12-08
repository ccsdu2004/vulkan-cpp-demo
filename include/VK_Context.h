#ifndef VK_CONEXT_H
#define VK_CONEXT_H
#include <functional>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "VK_ShaderSet.h"
#include "VK_Viewports.h"
#include "VK_Texture.h"
#include "VK_Buffer.h"
#include "VK_ImageView.h"

class VK_Buffer;
class VK_UniformBuffer;
class VK_Image;
class VK_Texture;
class VK_Context;
class VulKanAppData;

class VK_Context : public VK_Deleter
{
public:
    virtual bool createWindow(int width, int height, bool resize = false) = 0;
    virtual void setOnFrameSizeChanged(std::function<void(int, int)> cb) = 0;

    struct VK_Config {
        int maxFramsInFlight = 2;
        std::string pipelineCacheFile = "pipelineCache";
    };
    virtual bool initVulkanDevice(const VK_Config& config) = 0;
    virtual bool initVulkanContext(VK_ShaderSet* shaderSet) = 0;
    virtual bool initPipeline() = 0;
    virtual bool createCommandBuffers() = 0;
    virtual bool run() = 0;
public:
    virtual VkExtent2D getSwapChainExtent()const = 0;

    virtual VK_Viewports getViewports()const = 0;
    virtual void setViewports(const VK_Viewports& viewport) = 0;

    virtual void setClearColor(float r, float f, float b, float a) = 0;
    virtual void setClearDepthStencil(float depth, uint32_t stencil) = 0;

    virtual void setLogicalDeviceFeatures(const VkPhysicalDeviceFeatures& features) = 0;

    virtual VkPipelineColorBlendAttachmentState getColorBlendAttachmentState() = 0;
    virtual void setColorBlendAttachmentState(const VkPipelineColorBlendAttachmentState& state) = 0;

    virtual VkPipelineRasterizationStateCreateInfo getPipelineRasterizationStateCreateInfo() = 0;
    virtual void setPipelineRasterizationStateCreateInfo(const VkPipelineRasterizationStateCreateInfo& createInfo) = 0;

    virtual VkPipelineDepthStencilStateCreateInfo getPipelineDepthStencilStateCreateInfo() = 0;
    virtual void setPipelineDepthStencilStateCreateInfo(const VkPipelineDepthStencilStateCreateInfo& createInfo) = 0;

    virtual VkPipelineTessellationStateCreateInfo createPipelineTessellationStateCreateInfo() = 0;
    virtual VkPipelineTessellationStateCreateInfo getVkPipelineTessellationStateCreateInfo() = 0;
    virtual void setPipelineTessellationStateCreateInfo(const VkPipelineTessellationStateCreateInfo& createInfo) = 0;

    virtual void setDynamicState(VkDynamicState dynamicState) = 0;
public:
    virtual VK_ShaderSet* createShaderSet() = 0;

    virtual VK_Buffer* createVertexBuffer(const std::vector<float>& vertices, uint32_t count, const std::vector<uint16_t>& indices = std::vector<uint16_t>()) = 0;
    virtual VK_Buffer* createVertexBuffer(const std::vector<VK_Vertex>& vertices, const std::vector<uint16_t>& indices = std::vector<uint16_t>()) = 0;
    virtual void addBuffer(VK_Buffer* buffer) = 0;
    virtual void removeBuffer(VK_Buffer* buffer) = 0;

    virtual VK_Image* createImage(const std::string& image) = 0;
    virtual VK_Sampler* createSampler(const VkSamplerCreateInfo& samplerInfo) = 0;
    virtual VK_ImageView* createImageView(const VkImageViewCreateInfo& viewCreateInfo) = 0;
    virtual void addImageView(VK_ImageView* imageView) = 0;

    virtual VK_UniformBuffer* createUniformBuffer(uint32_t binding, uint32_t bufferSize) = 0;
    virtual void addUniformBuffer(VK_UniformBuffer* uniformBuffer) = 0;
public:
    virtual bool createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) = 0;
    virtual void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) = 0;
    virtual uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) = 0;
};

struct VK_ContextConfig {
    std::string name = "VK_Context";
    bool debug = false;
    std::function<void(int, int, int)> mouseCallback;

    VK_ContextConfig() = default;
    VK_ContextConfig(const VK_ContextConfig& config):
        name(config.name),
        debug(config.debug),
        mouseCallback(config.mouseCallback)
    {}
};
VK_Context* createVkContext(const VK_ContextConfig& config);

#endif // VULKANAPP_H


