#ifndef VK_CONTEXTIMPL_H
#define VK_CONTEXTIMPL_H
#include <vector>
#include <optional>
#include <algorithm>
#include <cstdint>
#include <set>
#include <vector>
#include <list>
#include <array>
#include <iostream>
#include <fstream>
#include "VK_Context.h"
#include "VK_UniformBuffer.h"

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VK_ContextImpl : public VK_Context
{
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    static void mouseButtonCallback(GLFWwindow* window, int button, int state, int mods);
public:
    VK_ContextImpl(const VK_ContextConfig &config);
    ~VK_ContextImpl();
public:
    void release()override;
public:
    bool createWindow(int width, int height, bool resize)override;
    bool initVulkan(const VK_Config& config)override;
    bool initPipeline(VK_ShaderSet* shaderSet)override;
    bool createCommandBuffers()override;
    bool run()override;
public:
    void setClearColor(float r, float g, float b, float a)override;
    void setClearDepthStencil(float depth, uint32_t stencil)override;

    VkPipelineColorBlendAttachmentState getColorBlendAttachmentState()override;
    void setColorBlendAttachmentState(const VkPipelineColorBlendAttachmentState& state)override;
public:
    VK_ShaderSet* createShaderSet()override;

    VK_Buffer* createVertexBuffer(const std::vector<VK_Vertex>& vertices, const std::vector<uint16_t>& indices = std::vector<uint16_t>())override;
    void removeBuffer(VK_Buffer* buffer)override;
    void addBuffer(VK_Buffer* buffer)override;

    VK_UniformBuffer* createUniformBuffer(uint32_t bufferSize)override;
    void setUniformBuffer(VK_UniformBuffer* uniformBuffer)override;
public:
    bool createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)override;
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)override;
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)override;
private:
    void recreateSwapChain();

    void cleanupSwapChain();
    void cleanup();

    bool createInstance();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    bool createDescriptorSetLayout();

    void createGraphicsPiplelineCache();
    bool saveGraphicsPiplineCache();
    bool createGraphicsPipeline();

    void createFramebuffers();

    void createCommandPool();

    void createDescriptorPool();

    void createSyncObjects();

    void drawFrame();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    bool isDeviceSuitable(VkPhysicalDevice device);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();
private:
    void initClearColorAndDepthStencil();
    void initColorBlendAttachmentState();
private:
    VK_ContextConfig appConfig;
    VK_Config vkConfig;

    GLFWwindow* window = nullptr;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = nullptr;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout = 0;

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline = 0;
    VkPipelineCache pipelineCache = 0;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    VkDescriptorPool descriptorPool = 0;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;

    bool framebufferResized = false;

    VK_ShaderSet* vkShaderSet = nullptr;
    std::list<VK_Buffer*> vkBuffers;

    VK_UniformBuffer* vkUniformBuffer = nullptr;

    bool vkNeedUpdateSwapChain = false;
    VkClearValue vkClearValue;
    VkPipelineColorBlendAttachmentState vkColorBlendAttachment{};
};


#endif // VK_CONTEXTIMPL_H
