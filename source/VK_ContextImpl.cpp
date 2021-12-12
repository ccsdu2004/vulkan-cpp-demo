#include <iostream>
#include <fstream>
#include <cstring>
#include <set>
#include "VK_ContextImpl.h"
#include "VK_ShaderSetImpl.h"
#include "VK_Buffer.h"
#include "VK_VertexBuffer.h"
#include "VK_UniformBufferImpl.h"
#include "VK_ImageImpl.h"
#include "VK_ImageViewImpl.h"
#include "VK_SamplerImpl.h"
#include "VK_Util.h"

VK_Context* createVkContext(const VK_ContextConfig& config)
{
    return new VK_ContextImpl(config);
}

VK_ContextImpl::VK_ContextImpl(const VK_ContextConfig& config):
    appConfig(config)
{
    glfwInit();
}

VK_ContextImpl::~VK_ContextImpl()
{
    glfwTerminate();
}

void VK_ContextImpl::release()
{
    delete this;
}

bool VK_ContextImpl::createWindow(int width, int height, bool resize)
{
    if(window)
        return false;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, resize);

    window = glfwCreateWindow(width, height, appConfig.name.data(), nullptr, nullptr);
    if(!window) {
        std::cerr << "create glfw window failed\n";
        return false;
    }
    glfwSwapInterval(1);

    glfwSetWindowUserPointer(window, this);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    return window != nullptr;
}

void VK_ContextImpl::setOnFrameSizeChanged(std::function<void (int, int)> cb)
{
    if(cb)
        windowSizeChangedCallback = cb;
}

bool VK_ContextImpl::initVulkanDevice(const VK_Config& config)
{
    vkConfig = config;

    vkValidationLayer = new VK_ValidationLayer(appConfig.debug);

    return createInstance() &&
           createSurface() &&
           pickPhysicalDevice() &&
           createLogicalDevice() && createCommandPool() &&
           createSwapChain() &&
           createSwapChainImageViews();
}

bool VK_ContextImpl::initVulkanContext(VK_ShaderSet* shaderSet)
{
    if(!shaderSet) {
        std::cerr << "invalid input shaderSet" << std::endl;
        return false;
    }
    vkShaderSet = shaderSet;
    createGraphicsPiplelineCache();

    createRenderPass();

    createDescriptorSetLayout();

    createDepthResources();
    createFramebuffers();
    createDescriptorPool();

    createDescriptorSets();

    createSyncObjects();

    initClearColorAndDepthStencil();
    initColorBlendAttachmentState();
    initRasterCreateInfo();
    initDepthStencilStateCreateInfo();
    initViewport();
    return true;
}

bool VK_ContextImpl::initPipeline()
{
    createGraphicsPipeline();
    return true;
}

bool VK_ContextImpl::run()
{
    //static double time = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        drawFrame();
        //std::cout << "fps:" << 1 / (glfwGetTime() - time) << std::endl;
        //time = glfwGetTime();
    }

    vkDeviceWaitIdle(device);
    cleanup();
    return true;
}

VkExtent2D VK_ContextImpl::getSwapChainExtent() const
{
    return vkSwapChainExtent;
}

VK_Viewports VK_ContextImpl::getViewports() const
{
    return vkViewports;
}

void VK_ContextImpl::setViewports(const VK_Viewports &viewports)
{
    if(vkViewports != viewports) {
        vkViewports = viewports;
        vkNeedUpdateSwapChain = true;
    }
}

void VK_ContextImpl::setClearColor(float r, float g, float b, float a)
{
    vkClearValue.color.float32[0] = std::clamp<float>(r, 0.0f, 1.0f);
    vkClearValue.color.float32[1] = std::clamp<float>(g, 0.0f, 1.0f);
    vkClearValue.color.float32[2] = std::clamp<float>(b, 0.0f, 1.0f);
    vkClearValue.color.float32[3] = std::clamp<float>(a, 0.0f, 1.0f);

    vkNeedUpdateSwapChain = true;
}

void VK_ContextImpl::setClearDepthStencil(float depth, uint32_t stencil)
{
    vkClearValue.depthStencil.depth = std::clamp<float>(depth, 0.0f, 1.0f);
    vkClearValue.depthStencil.stencil = stencil;

    vkNeedUpdateSwapChain = true;
}

void VK_ContextImpl::setLogicalDeviceFeatures(const VkPhysicalDeviceFeatures &features)
{
    logicalFeatures = features;
}

VkPipelineColorBlendAttachmentState VK_ContextImpl::getColorBlendAttachmentState()
{
    return vkColorBlendAttachment;
}

void VK_ContextImpl::setColorBlendAttachmentState(const VkPipelineColorBlendAttachmentState &state)
{
    int size = sizeof(VkPipelineColorBlendAttachmentState);
    if(memcmp(&vkColorBlendAttachment, &state, size)) {
        vkColorBlendAttachment = state;
        vkNeedUpdateSwapChain = true;
    }
}

VkPipelineRasterizationStateCreateInfo VK_ContextImpl::getPipelineRasterizationStateCreateInfo()
{
    return vkPipelineRasterizationStateCreateInfo;
}

void VK_ContextImpl::setPipelineRasterizationStateCreateInfo(const VkPipelineRasterizationStateCreateInfo &createInfo)
{
    vkPipelineRasterizationStateCreateInfo = createInfo;
    vkNeedUpdateSwapChain = true;
}

VkPipelineDepthStencilStateCreateInfo VK_ContextImpl::getPipelineDepthStencilStateCreateInfo()
{
    return vkPipelineDepthStencilStateCreateInfo;
}

void VK_ContextImpl::setPipelineDepthStencilStateCreateInfo(const VkPipelineDepthStencilStateCreateInfo &createInfo)
{
    vkPipelineDepthStencilStateCreateInfo = createInfo;
    vkNeedUpdateSwapChain = true;
}

VkPipelineTessellationStateCreateInfo VK_ContextImpl::createPipelineTessellationStateCreateInfo()
{
    VkPipelineTessellationStateCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    createInfo.patchControlPoints = 32;
    createInfo.flags = 0;
    createInfo.pNext = nullptr;
    return createInfo;
}

VkPipelineTessellationStateCreateInfo VK_ContextImpl::getVkPipelineTessellationStateCreateInfo()
{
    return vkPipelineTessellationStateCreateInfo.value();
}

void VK_ContextImpl::setPipelineTessellationStateCreateInfo(const VkPipelineTessellationStateCreateInfo &createInfo)
{
    vkPipelineTessellationStateCreateInfo = std::optional<VkPipelineTessellationStateCreateInfo>(createInfo);
}

void VK_ContextImpl::setDynamicState(VkDynamicState dynamicState)
{
    vkDynamicStates.emplace_back(dynamicState);
}

VkPipelineDynamicStateCreateInfo VK_ContextImpl::createDynamicStateCreateInfo(VkPipelineDynamicStateCreateFlags flags)
{
    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
    pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    pipelineDynamicStateCreateInfo.pDynamicStates = vkDynamicStates.data();
    pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(vkDynamicStates.size());
    pipelineDynamicStateCreateInfo.flags = flags;
    return pipelineDynamicStateCreateInfo;
}

void VK_ContextImpl::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    auto context = reinterpret_cast<VK_ContextImpl*>(glfwGetWindowUserPointer(window));
    if(context->windowSizeChangedCallback)
        context->windowSizeChangedCallback(width, height);
    context->framebufferResized = true;
}

void VK_ContextImpl::mouseButtonCallback(GLFWwindow *window, int button, int state, int mods)
{
    auto context = reinterpret_cast<VK_ContextImpl*>(glfwGetWindowUserPointer(window));
    if(context->appConfig.mouseCallback)
        context->appConfig.mouseCallback(button, state, mods);
}

void VK_ContextImpl::cleanupSwapChain()
{
    vkDepthImageView->release();
    vkDepthImage->release();

    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);

    std::for_each(swapChainImageViews.begin(), swapChainImageViews.end(), [](VK_ImageView * view) {
        view->release();
    });

    vkDestroySwapchainKHR(device, swapChain, nullptr);

    for(auto buffer : vkUniformBuffers)
        buffer->clearBuffer();

    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}

void VK_ContextImpl::cleanup()
{
    if(vkShaderSet)
        vkShaderSet->release();

    while(true) {
        auto itr = vkBuffers.begin();
        if(itr == vkBuffers.end())
            break;
        (*itr)->release();
    }

    vkBuffers.clear();

    saveGraphicsPiplineCache();
    vkDestroyPipelineCache(device, pipelineCache, nullptr);

    cleanupSwapChain();

    cleanVulkanObjectContainer(vkUniformBuffers);

    cleanVulkanObjectContainer(vkSamplerList);
    cleanVulkanObjectContainer(vkImageViewList);
    cleanVulkanObjectContainer(vkUnusedImageViewList);
    cleanVulkanObjectContainer(vkImageList);

    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

    for (int i = 0; i < vkConfig.maxFramsInFlight; i++) {
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(device, commandPool, nullptr);

    vkDestroyDevice(device, nullptr);

    vkValidationLayer->cleanup(instance);
    vkValidationLayer->release();

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();
}

void VK_ContextImpl::recreateSwapChain()
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device);

    cleanupSwapChain();

    createSwapChain();
    createSwapChainImageViews();
    createRenderPass();
    createGraphicsPipeline();

    createDepthResources();

    createFramebuffers();

    for(auto buffer : vkUniformBuffers)
        buffer->initBuffer(swapChainImageViews.size());

    createDescriptorPool();

    createDescriptorSets();

    createCommandBuffers();

    imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);
}

bool VK_ContextImpl::createInstance()
{
    if (!vkValidationLayer->appendValidationLayerSupport()) {
        std::cerr << "validation layers requested, but not available!" << std::endl;
        return false;
    }

    VkApplicationInfo appInfo;
    appInfo.pNext = nullptr;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appConfig.name.data();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Vulkan Framework";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    vkValidationLayer->adjustVkInstanceCreateInfo(createInfo);

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        std::cerr << "failed to create instance!" << std::endl;
        return false;
    }

    vkValidationLayer->setupDebugMessenger(instance);
    return true;
}

bool VK_ContextImpl::createSurface()
{
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        std::cerr << "failed to create window surface!" << std::endl;
        return false;
    }
    return true;
}

bool VK_ContextImpl::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        std::cerr << "failed to find GPUs with Vulkan support!" << std::endl;
        return false;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    std::vector<VkPhysicalDevice> deviceList;

    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            deviceList.push_back(device);
        }
    }

    if (deviceList.empty()) {
        std::cerr << "failed to find a suitable GPU!" << std::endl;
        return false;
    }

    for(auto device : deviceList) {
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        std::cout << "device name:" << deviceProperties.deviceName << std::endl;
    }

    physicalDevice = deviceList[0];

    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    return true;
}

bool VK_ContextImpl::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.multiViewport = VK_TRUE;
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &logicalFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    vkValidationLayer->adjustVkDeviceCreateInfo(createInfo);

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        std::cerr << "failed to create logical device!" << std::endl;
        return false;
    }

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
    return true;
}

bool VK_ContextImpl::createSwapChain()
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        std::cerr << "failed to create swap chain" << std::endl;
        return false;
    }

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount, VK_NULL_HANDLE);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    vkSwapChainExtent = extent;
    return true;
}

bool VK_ContextImpl::createSwapChainImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        VkImageViewCreateInfo viewCreateInfo = VK_ImageView::createImageViewCreateInfo(swapChainImages[i], swapChainImageFormat);
        viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

        VK_ImageViewImpl* view = new VK_ImageViewImpl(device, this);
        view->setRemoveFromContainerWhenRelease(false);
        if(!view->create(viewCreateInfo)) {
            std::cerr << "failed to create image views" << std::endl;
            return false;
        }

        swapChainImageViews[i] = view;
    }
    return true;
}

void VK_ContextImpl::createRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        std::cerr << "failed to create render pass" << std::endl;
    }
}

bool VK_ContextImpl::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = vkShaderSet->getDescriptorSetLayoutBindingCount();
    layoutInfo.pBindings = vkShaderSet->getDescriptorSetLayoutBindingData();
    layoutInfo.pNext = nullptr;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        std::cerr << "failed to create descriptor set layout!" << std::endl;
        return false;
    }

    return true;
}

bool VK_ContextImpl::isValidPipelineCacheData(const std::string& filename, const char *buffer, uint32_t size)
{
    if(size > 32) {
        uint32_t header = 0;
        uint32_t version = 0;
        uint32_t vendor = 0;
        uint32_t deviceID = 0;
        uint8_t pipelineCacheUUID[VK_UUID_SIZE] = {};

        memcpy(&header, (uint8_t *)buffer + 0, 4);
        memcpy(&version, (uint8_t *)buffer + 4, 4);
        memcpy(&vendor, (uint8_t *)buffer + 8, 4);
        memcpy(&deviceID, (uint8_t *)buffer + 12, 4);
        memcpy(pipelineCacheUUID, (uint8_t *)buffer + 16, VK_UUID_SIZE);

        if (header <= 0) {
            std::cerr << "bad pipeline cache data header length in " << vkConfig.pipelineCacheFile << std::endl;
            return false;
        }

        if (version != VK_PIPELINE_CACHE_HEADER_VERSION_ONE) {
            std::cerr << "unsupported cache header version in " << filename << std::endl;
            std::cerr << "cache contains: 0x" << std::hex << version << std::endl;
        }

        if (vendor != deviceProperties.vendorID) {
            std::cerr << "vendor id mismatch in " << filename << std::endl;
            std::cerr << "cache contains: 0x" << std::hex << vendor << std::endl;
            std::cerr << "driver expects: 0x" << deviceProperties.vendorID << std::endl;
            return false;
        }

        if (deviceID != deviceProperties.deviceID) {
            std::cerr << "device id mismatch in " << filename << std::endl;
            std::cerr << "cache contains: 0x" << std::hex << deviceID << std::endl;
            std::cerr << "driver expects: 0x" << deviceProperties.deviceID << std::endl;
            return false;
        }

        if (memcmp(pipelineCacheUUID, deviceProperties.pipelineCacheUUID, sizeof(pipelineCacheUUID)) != 0) {
            std::cerr << "uuid mismatch in " << filename << std::endl;
            std::cerr << "cache contains: " << std::endl;

            auto fn = [](uint8_t* uuid) {
                for(int i = 0; i < 16; i++) {
                    std::cout << (int)uuid[i] << " ";
                    if(i % 4 == 3)
                        std::cerr << std::endl;
                }
                std::cerr << std::endl;
            };
            fn(pipelineCacheUUID);
            std::cerr << "driver expects:" << std::endl;
            fn(deviceProperties.pipelineCacheUUID);
            return false;
        }

        return true;
    }
    return false;
}

void VK_ContextImpl::createGraphicsPiplelineCache()
{
    auto buffer = readDataFromFile(vkConfig.pipelineCacheFile);
    bool valid = appConfig.debug && isValidPipelineCacheData(vkConfig.pipelineCacheFile, buffer.data(), buffer.size());

    VkPipelineCacheCreateInfo PipelineCacheCreateInfo = {};
    PipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    PipelineCacheCreateInfo.pNext = nullptr;
    PipelineCacheCreateInfo.initialDataSize = valid ? buffer.size() : 0;
    PipelineCacheCreateInfo.pInitialData = valid ? buffer.data() : nullptr;

    if (vkCreatePipelineCache(device, &PipelineCacheCreateInfo, nullptr, &pipelineCache) != VK_SUCCESS)
        std::cerr << "creating pipeline cache error" << std::endl;
}

bool VK_ContextImpl::saveGraphicsPiplineCache()
{
    size_t cacheSize = 0;

    if (vkGetPipelineCacheData(device, pipelineCache, &cacheSize, nullptr) != VK_SUCCESS) {
        std::cerr << "getting cache size fail from pipelinecache" << std::endl;
        return false;
    }

    auto cacheData = std::vector<char>(sizeof(char) * cacheSize, 0);

    if(vkGetPipelineCacheData(device, pipelineCache, &cacheSize, &cacheData[0]) != VK_SUCCESS) {
        std::cerr << "getting cache fail from pipelinecache" << std::endl;
        return false;
    }

    std::ofstream stream(vkConfig.pipelineCacheFile, std::ios::binary);
    if(stream.is_open()) {
        stream.write(cacheData.data(), cacheData.size());
        stream.close();
    } else {
        std::cerr << "open pipeline cache data target file failed!" << std::endl;
        return false;
    }
    return true;
}

VK_ShaderSet *VK_ContextImpl::createShaderSet()
{
    return new VK_ShaderSetImpl(device);
}

VK_Buffer *VK_ContextImpl::createVertexBuffer(const std::vector<float> &vertices, uint32_t count, const std::vector<uint16_t> &indices)
{
    auto vertexBuffer = new VK_VertexBuffer(this, device);
    vertexBuffer->create(vertices, count, indices);
    return vertexBuffer;
}

VK_Buffer *VK_ContextImpl::createVertexBuffer(const std::vector<VK_Vertex> &vertices, const std::vector<uint16_t>& indices)
{
    auto vertexBuffer = new VK_VertexBuffer(this, device);
    vertexBuffer->create(vertices, indices);
    return vertexBuffer;
}

void VK_ContextImpl::removeBuffer(VK_Buffer * buffer)
{
    vkBuffers.remove(buffer);
}

void VK_ContextImpl::addBuffer(VK_Buffer * buffer)
{
    if(buffer)
        vkBuffers.push_back(buffer);
}

VK_Image *VK_ContextImpl::createImage(const std::string &image)
{
    auto ptr = new VK_ImageImpl(device, this);
    if(!ptr->load(image)) {
        delete ptr;
        return nullptr;
    }

    vkImageList.push_back(ptr);
    return ptr;
}

void VK_ContextImpl::onReleaseImage(VK_Image *image)
{
    if(image)
        vkImageList.remove(image);
}

VK_Sampler *VK_ContextImpl::createSampler(const VkSamplerCreateInfo &samplerInfo)
{
    auto sampler = new VK_SamplerImpl(device, this);
    if(!sampler->create(samplerInfo)) {
        delete sampler;
        return nullptr;
    }
    vkSamplerList.push_back(sampler);
    return sampler;
}

void VK_ContextImpl::removeSampler(VK_Sampler *sampler)
{
    vkSamplerList.remove(sampler);
}

VK_ImageView *VK_ContextImpl::createImageView(const VkImageViewCreateInfo &viewCreateInfo)
{
    auto imageView = new VK_ImageViewImpl(device, this);
    if(!imageView->create(viewCreateInfo)) {
        delete imageView;
        return nullptr;
    }
    vkImageViewList.push_back(imageView);
    return imageView;
}

void VK_ContextImpl::addImageView(VK_ImageView *imageView)
{
    if(imageView) {
        auto find = std::find(vkImageViewList.begin(), vkImageViewList.end(), imageView);
        if(find != vkImageViewList.end())
            return;
        vkImageViewList.push_back(imageView);
        vkUnusedImageViewList.erase(imageView);
    }
}

void VK_ContextImpl::removeImageView(VK_ImageView *imageView)
{
    vkImageViewList.remove(imageView);
    vkUnusedImageViewList.erase(imageView);
}

VK_UniformBuffer *VK_ContextImpl::createUniformBuffer(uint32_t binding, uint32_t bufferSize)
{
    auto buffer = new VK_UniformBufferImpl(this, device, binding, bufferSize);
    buffer->initBuffer(swapChainImageViews.size());
    buffer->setRemoveFromContainerWhenRelease(true);
    return buffer;
}

void VK_ContextImpl::addUniformBuffer(VK_UniformBuffer *uniformBuffer)
{
    if(uniformBuffer) {
        auto find = std::find(vkUniformBuffers.begin(), vkUniformBuffers.end(), uniformBuffer);
        if(find == vkUniformBuffers.end())
            vkUniformBuffers.push_back(uniformBuffer);
    }
}

void VK_ContextImpl::removeUniformBuffer(VK_UniformBuffer *uniformBuffer)
{
    vkUniformBuffers.remove(uniformBuffer);
}

bool VK_ContextImpl::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer & buffer, VkDeviceMemory & bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.pNext = nullptr;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        std::cerr << "failed to create buffer!" << std::endl;
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
    allocInfo.pNext = nullptr;

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        std::cerr << "failed to allocate buffer memory!" << std::endl;
        return false;
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
    return true;
}

void VK_ContextImpl::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

bool VK_ContextImpl::createGraphicsPipeline()
{
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr;

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = vkShaderSet->getAttributeDescriptionCount();
    vertexInputInfo.pVertexBindingDescriptions = vkShaderSet->getBindingDescription();
    vertexInputInfo.pVertexAttributeDescriptions = vkShaderSet->getAttributeDescriptionData();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    if(vkPipelineTessellationStateCreateInfo.has_value()) {
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
    } else
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = vkViewports.getViewportCount();
    viewportState.pViewports = vkViewports.getViewportData();
    viewportState.scissorCount = vkViewports.getViewportCount();
    viewportState.pScissors = vkViewports.getScissorData();

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &vkColorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pNext = nullptr;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        std::cerr << "failed to create pipeline layout!" << std::endl;
        return false;
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = vkShaderSet->getCreateInfoCount();
    pipelineInfo.pStages = vkShaderSet->getCreateInfoData();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &vkPipelineRasterizationStateCreateInfo;
    pipelineInfo.pDepthStencilState = &vkPipelineDepthStencilStateCreateInfo;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;

    if(!vkPipelineTessellationStateCreateInfo.has_value())
        pipelineInfo.pTessellationState = nullptr;
    else
        pipelineInfo.pTessellationState = &vkPipelineTessellationStateCreateInfo.value();

    auto dynamicState = createDynamicStateCreateInfo();
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.pNext = nullptr;

    if (vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        std::cerr << "failed to create graphics pipeline!" << std::endl;
        return false;
    }

    return true;
}

void VK_ContextImpl::createFramebuffers()
{
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        std::array<VkImageView, 2> attachments = {
            swapChainImageViews[i]->getImageView(),
            vkDepthImageView->getImageView()
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = vkSwapChainExtent.width;
        framebufferInfo.height = vkSwapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

bool VK_ContextImpl::createCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.pNext = nullptr;
    poolInfo.flags = 0;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        std::cerr << "failed to create command pool!" << std::endl;
        return false;
    }
    return true;
}

void VK_ContextImpl::createDepthResources()
{
    VkFormat depthFormat = findDepthFormat();

    vkDepthImage = new VK_ImageImpl(device, this);
    bool ok = vkDepthImage->createImage(vkSwapChainExtent.width, vkSwapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if(!ok)
        std::cerr << "create depth image failed\n";

    auto createInfo = VK_ImageView::createImageViewCreateInfo(vkDepthImage->getImage(), depthFormat);
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    vkDepthImageView = new VK_ImageViewImpl(device, this);
    vkDepthImageView->create(createInfo);
}

VkFormat VK_ContextImpl::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    std::cerr << "failed to find supported format!" << std::endl;
    return VkFormat();
}

VkFormat VK_ContextImpl::findDepthFormat()
{
    return findSupportedFormat(
    {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
    VK_IMAGE_TILING_OPTIMAL,
    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

bool VK_ContextImpl::hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void VK_ContextImpl::createDescriptorPool()
{
    vkShaderSet->updateDescriptorPoolSize(swapChainImageViews.size());

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(swapChainImages.size());

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    if(vkShaderSet->getDescriptorPoolSizeCount() > 0) {
        poolInfo.poolSizeCount = vkShaderSet->getDescriptorPoolSizeCount();
        poolInfo.pPoolSizes = vkShaderSet->getDescriptorPoolSizeData();
    } else {
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
    }
    poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        std::cerr << "failed to create descriptor pool!" << std::endl;
    }
}

void VK_ContextImpl::createDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(swapChainImages.size());

    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        std::cerr << "failed to allocate descriptor sets" << std::endl;
        return;
    }

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        std::vector<VkWriteDescriptorSet> descriptorWrites;
        for(auto buffer : vkUniformBuffers)
            descriptorWrites.push_back(buffer->createWriteDescriptorSet(i, descriptorSets[i]));

        for(auto imageView : vkImageViewList) {
            descriptorWrites.push_back(imageView->createWriteDescriptorSet(descriptorSets[i]));
        }

        vkUpdateDescriptorSets(device, descriptorWrites.size(), &descriptorWrites[0], 0, nullptr);
    }
}

bool VK_ContextImpl::createCommandBuffers()
{
    commandBuffers.resize(swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();
    allocInfo.pNext = nullptr;

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        std::cerr << "failed to allocate command buffers!" << std::endl;
        return false;
    }

    for (size_t i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = vkSwapChainExtent;

        std::array<VkClearValue, 2> clearValues{};
        memcpy((char*)&clearValues[0], &vkClearValue, sizeof(vkClearValue));
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
        vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

        for(auto itr = vkBuffers.begin(); itr != vkBuffers.end(); itr++)
            (*itr)->render(commandBuffers[i]);

        vkCmdEndRenderPass(commandBuffers[i]);

        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            std::cout << "failed to record command buffer!" << std::endl;
        }
    }
    return true;
}

void VK_ContextImpl::createSyncObjects()
{
    auto count = vkConfig.maxFramsInFlight;
    imageAvailableSemaphores.resize(count);
    renderFinishedSemaphores.resize(count);
    inFlightFences.resize(count);
    imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < vkConfig.maxFramsInFlight; i++) {
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
            std::cerr << "failed to create synchronization objects for a frame!" << std::endl;
        }
    }
}

void VK_ContextImpl::drawFrame()
{
    if(vkNeedUpdateSwapChain) {
        recreateSwapChain();
        vkNeedUpdateSwapChain = false;
    }

    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    for(auto buffer : vkUniformBuffers)
        buffer->update(imageIndex);

    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(device, 1, &inFlightFences[currentFrame]);

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR ||
        result == VK_SUBOPTIMAL_KHR ||
        framebufferResized) {
        framebufferResized = false;
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        std::cerr << "failed to present swap chain image!" << std::endl;
    }

    currentFrame = (currentFrame + 1) % vkConfig.maxFramsInFlight;
}

VkSurfaceFormatKHR VK_ContextImpl::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR VK_ContextImpl::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VK_ContextImpl::chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

SwapChainSupportDetails VK_ContextImpl::querySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

bool VK_ContextImpl::isDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices list = findQueueFamilies(device);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    return list.isComplete() && extensionsSupported && swapChainAdequate && deviceFeatures.samplerAnisotropy;
}

bool VK_ContextImpl::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices VK_ContextImpl::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

std::vector<const char *> VK_ContextImpl::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (appConfig.debug) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void VK_ContextImpl::initClearColorAndDepthStencil()
{
    vkClearValue = {{{0.0f, 0.0f, 0.0f, 0.0f}}};
}

void VK_ContextImpl::initColorBlendAttachmentState()
{
    vkColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    vkColorBlendAttachment.blendEnable = VK_FALSE;
}

void VK_ContextImpl::initRasterCreateInfo()
{
    vkPipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    vkPipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    vkPipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    vkPipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    vkPipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
    vkPipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    vkPipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    vkPipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
}

void VK_ContextImpl::initDepthStencilStateCreateInfo()
{
    vkPipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    vkPipelineDepthStencilStateCreateInfo.pNext = NULL;
    vkPipelineDepthStencilStateCreateInfo.flags = 0;
    vkPipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    vkPipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    vkPipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    vkPipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    vkPipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
    vkPipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;
    vkPipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    vkPipelineDepthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
    vkPipelineDepthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
    vkPipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
    vkPipelineDepthStencilStateCreateInfo.back.compareMask = 0;
    vkPipelineDepthStencilStateCreateInfo.back.reference = 0;
    vkPipelineDepthStencilStateCreateInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
    vkPipelineDepthStencilStateCreateInfo.back.writeMask = 0;
    vkPipelineDepthStencilStateCreateInfo.front = vkPipelineDepthStencilStateCreateInfo.back;
}

void VK_ContextImpl::initViewport()
{
    vkViewports.addViewport(VK_Viewports::createViewport(vkSwapChainExtent.width, vkSwapChainExtent.height));
}

uint32_t VK_ContextImpl::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    std::cerr << "failed to find suitable memory type!" << std::endl;
    return ~0;
}

void VK_ContextImpl::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        std::cerr << "unsupported layout transition!" << std::endl;
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    endSingleTimeCommands(commandBuffer);
}

void VK_ContextImpl::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    endSingleTimeCommands(commandBuffer);
}

VkCommandBuffer VK_ContextImpl::beginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VK_ContextImpl::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void VK_ContextImpl::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        std::cerr << "texture image format does not support linear blitting!" << std::endl;
    }

    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        VkImageBlit blit = {};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer,
                       image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &blit,
                       VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        if (mipWidth > 1)
            mipWidth /= 2;
        if (mipHeight > 1)
            mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);

    endSingleTimeCommands(commandBuffer);
}
