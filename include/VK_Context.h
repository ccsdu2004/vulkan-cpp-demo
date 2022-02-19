#ifndef VK_CONEXT_H
#define VK_CONEXT_H
#include <functional>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VK_ShaderSet.h>
#include <VK_Texture.h>
#include <VK_Buffer.h>
#include <VK_ImageView.h>

class VK_Buffer;
class VK_UniformBuffer;
class VK_IndirectBuffer;
class VK_InstanceBuffer;
class VK_CommandPool;
class VK_Image;
class VK_Texture;
class VK_Context;
class VK_PipelineCache;
class VK_DynamicState;
class VK_Pipeline;
class VulKanAppData;

class VK_Context : public VK_Deleter
{
public:
    virtual bool createWindow(int width, int height, bool resize = false) = 0;
    virtual void setOnFrameSizeChanged(std::function<void(int, int)> cb) = 0;

    struct VK_Config {
        int maxFramsInFlight = 2;
        std::string pipelineCacheFile = "pipelineCache";
        std::vector<const char *> enabledExtensions;
    };

    virtual VkAllocationCallbacks *getAllocation() = 0;

    virtual bool initVulkanDevice(const VK_Config &config) = 0;
    virtual VkDevice getDevice()const = 0;
    virtual VK_PipelineCache *getPipelineCache()const = 0;
    virtual VkPhysicalDevice getPhysicalDevice()const = 0;
    virtual size_t getSwapImageCount()const = 0;

    virtual bool initVulkanContext() = 0;
    virtual VkQueue getGraphicQueue()const = 0;
    virtual VkRenderPass getRenderPass()const = 0;

    virtual VK_Pipeline *createPipeline(VK_ShaderSet *shaderSet) = 0;
    virtual bool createCommandBuffers() = 0;
    virtual bool run() = 0;
public:
    virtual VkExtent2D getSwapChainExtent()const = 0;
    virtual VkFormat getSwapChainFormat()const = 0;

    virtual void setClearColor(float r, float f, float b, float a) = 0;
    virtual void setClearDepthStencil(float depth, uint32_t stencil) = 0;

    virtual void setLogicalDeviceFeatures(const VkPhysicalDeviceFeatures &features) = 0;

    virtual void captureScreenShot() = 0;
public:
    virtual VK_ShaderSet *createShaderSet() = 0;

    virtual VK_Buffer *createVertexBuffer(const std::vector<float> &vertices, uint32_t count,
                                          const std::vector<uint32_t> &indices = std::vector<uint32_t>(), bool indirectDraw = false) = 0;
    virtual VK_Buffer *createVertexBuffer(const std::vector<VK_Vertex> &vertices,
                                          const std::vector<uint32_t> &indices = std::vector<uint32_t>(), bool indirectDraw = false) = 0;
    virtual VK_Buffer *createVertexBuffer(const std::string &filename, bool zero = true,
                                          bool indirectDraw = false) = 0;

    virtual VK_Buffer *createIndirectBuffer(uint32_t instanceCount, uint32_t oneInstanceSize,
                                            uint32_t vertexCount) = 0;
    virtual VK_Buffer *createInstanceBuffer(uint32_t count, uint32_t itemSize, const char *data,
                                            uint32_t bind) = 0;

    virtual void addBuffer(VK_Buffer *buffer) = 0;
    virtual void removeBuffer(VK_Buffer *buffer) = 0;

    virtual VK_Image *createImage(const std::string &image) = 0;
    virtual VK_Sampler *createSampler(const VkSamplerCreateInfo &samplerInfo) = 0;
    virtual VK_ImageView *createImageView(const VkImageViewCreateInfo &viewCreateInfo,
                                          uint32_t mipLevels = 1) = 0;
public:
    virtual bool createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                              VkMemoryPropertyFlags properties,
                              VkBuffer &buffer, VkDeviceMemory &bufferMemory) = 0;
    virtual void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) = 0;

    virtual VK_CommandPool *getCommandPool()const = 0;
};

struct VK_ContextConfig {
    std::string name = "VK_Context";
    bool debug = false;
    std::function<void(int, int, int)> mouseCallback;

    VK_ContextConfig() = default;
    VK_ContextConfig(const VK_ContextConfig &config):
        name(config.name),
        debug(config.debug),
        mouseCallback(config.mouseCallback)
    {}
};
VK_Context *createVkContext(const VK_ContextConfig &config);

#endif // VULKANAPP_H


