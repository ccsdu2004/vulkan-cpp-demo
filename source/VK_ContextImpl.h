#ifndef VK_CONTEXTIMPL_H
#define VK_CONTEXTIMPL_H
#include <vector>
#include <optional>
#include <algorithm>
#include <cstdint>
#include <set>
#include <memory>
#include <vector>
#include <list>
#include <array>
#include <iostream>
#include "VK_Context.h"
#include "VK_UniformBuffer.h"
#include "VK_ImageImpl.h"
#include "VK_ImageViewImpl.h"
#include "VK_ValidationLayer.h"
#include "VK_Allocator.h"
#include "VK_DynamicStateImpl.h"
#include "VK_PipelineCacheImpl.h"
#include "VK_PipelineImpl.h"
#include "VK_RenderPass.h"
#include "VK_CommandPool.h"
#include "VK_Util.h"

class VK_ContextImpl : public VK_Context
{
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
    static void mouseButtonCallback(GLFWwindow *window, int button, int state, int mods);
public:
    VK_ContextImpl(const VK_ContextConfig &config);
    ~VK_ContextImpl();
public:
    VkAllocationCallbacks *getAllocation()override;
    void release()override;
public:
    bool createWindow(int width, int height, bool resize)override;
    void setOnFrameSizeChanged(std::function<void(int, int)> cb) override;

    bool initVulkanDevice(const VK_Config &config)override;
    VkDevice getDevice()const override;
    VK_PipelineCache *getPipelineCache()const override;
    VkPhysicalDevice getPhysicalDevice()const override;
    size_t getSwapImageCount()const override;
    VkSampleCountFlagBits getSampleCountFlagBits()const;

    bool initVulkanContext()override;
    VkRenderPass getRenderPass()const override;
    VkQueue getGraphicQueue()const override;

    VK_Pipeline *createPipeline(VK_ShaderSet *shaderSet) override;
    void addPipeline(VK_PipelineImpl *pipeline);
    bool createCommandBuffers()override;
    bool run()override;
public:
    VkExtent2D getSwapChainExtent()const override;
    VkFormat getSwapChainFormat()const override;
public:
    void setClearColor(float r, float g, float b, float a)override;
    void setClearDepthStencil(float depth, uint32_t stencil)override;

    void setLogicalDeviceFeatures(const VkPhysicalDeviceFeatures &features)override;
public:
    VK_ShaderSet *createShaderSet()override;

    VK_Buffer *createVertexBuffer(const std::vector<float> &vertices, uint32_t count,
                                  const std::vector<uint32_t> &indices = std::vector<uint32_t>(), bool indirectDraw = false)override;
    VK_Buffer *createVertexBuffer(const std::vector<VK_Vertex> &vertices,
                                  const std::vector<uint32_t> &indices = std::vector<uint32_t>(), bool indirectDraw = false)override;
    VK_Buffer *createVertexBuffer(const std::string &filename, bool zero = true, bool indirectDraw = false)override;

    VK_Buffer *createIndirectBuffer(uint32_t instanceCount, uint32_t oneInstanceSize,
                                    uint32_t vertexCount) override;
    VK_Buffer *createInstanceBuffer(uint32_t count, uint32_t itemSize, const char *data,
                                    uint32_t bind) override;

    void removeBuffer(VK_Buffer *buffer)override;
    void addBuffer(VK_Buffer *buffer)override;
public:
    VK_Image *createImage(const std::string &image)override;
    void onReleaseImage(VK_Image *image);

    VK_Sampler *createSampler(const VkSamplerCreateInfo &samplerInfo)override;
    void removeSampler(VK_Sampler *sampler);

    VK_ImageView *createImageView(const VkImageViewCreateInfo &viewCreateInfo,
                                  uint32_t mipLevels = 1)override;
    void removeImageView(VK_ImageView *imageView);
public:
    bool createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                      VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory)override;
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)override;

    VK_CommandPool* getCommandPool()const override;
private:
    void recreateSwapChain();

    void cleanupSwapChain();
    void cleanup();

    bool createInstance();
    bool createSurface();
    bool pickPhysicalDevice();
    bool createLogicalDevice();
    bool createSwapChain();
    bool createSwapChainImageViews();
    bool createPipelineCache();
    void createRenderPass();

    bool createGraphicsPipeline();

    void createFramebuffers();

    bool createCommandPool();

    void createColorResources();
    void createDepthResources();

    void createSyncObjects();
    bool drawFrame();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    bool isDeviceSuitable(VkPhysicalDevice device);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    std::vector<const char *> getRequiredExtensions();
private:
    VK_ContextConfig appConfig;
    VK_Config vkConfig;

    std::vector<const char *> deviceExtensions;

    VK_Allocator *vkAllocator = nullptr;

    GLFWwindow *window = nullptr;
    std::function<void(int, int)> windowSizeChangedCallback;

    VkInstance instance = nullptr;
    VK_ValidationLayer *vkValidationLayer = nullptr;

    VkSurfaceKHR surface = nullptr;

    VkPhysicalDeviceFeatures deviceFeatures{};
    VkPhysicalDeviceFeatures logicalFeatures{};
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    VkPhysicalDeviceProperties deviceProperties;
    VkDevice device = nullptr;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D vkSwapChainExtent;

    std::vector<VK_ImageView *> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VK_RenderPass *renderPass = nullptr;

    VK_PipelineCacheImpl *vkPipelineCache = nullptr;
    std::list<VK_PipelineImpl *> pipelines;

    VK_CommandPool* commandPool = nullptr;
    std::vector<VkCommandBuffer> commandBuffers;

    VK_ImageImpl *vkColorImage = nullptr;
    VK_ImageViewImpl *vkColorImageView = nullptr;

    VK_ImageImpl *vkDepthImage = nullptr;
    VK_ImageViewImpl *vkDepthImageView = nullptr;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;

    bool framebufferResized = false;

    std::list<VK_ShaderSet *> vkShaders;
    std::list<VK_Buffer *> vkBuffers;

    bool needUpdateSwapChain = false;

    VkClearValue vkClearValue = {{{0.0f, 0.0f, 0.0f, 0.0f}}};

    std::list<VK_Image *> vkImageList;
    std::list<VK_Sampler *> vkSamplerList;
    std::list<VK_ImageView *> vkImageViewList;
};

#endif // VK_CONTEXTIMPL_H
