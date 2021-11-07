#ifndef VK_CONEXT_H
#define VK_CONEXT_H
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <functional>
#include "VK_ShaderSet.h"
#include "VK_Buffer.h"
#include "source/VK_ShaderSetImpl.h"

class VK_Buffer;
class VK_Context;
class VulKanAppData;

class VK_Context : public VK_Deleter
{
public:
    virtual bool createWindow(int width, int height, bool resize = false) = 0;

    struct VK_Config {
        int maxFramsInFlight = 2;
    };
    virtual bool initVulkan(const VK_Config& config) = 0;
    virtual bool initPipeline(VK_ShaderSet* shaderSet) = 0;
    virtual bool createCommandBuffers() = 0;
    virtual bool run() = 0;
public:
    virtual void setClearColor(float r, float f, float b, float a) = 0;
    virtual void setClearDepthStencil(float depth, uint32_t stencil) = 0;
public:
    virtual VK_ShaderSet* createShaderSet() = 0;

    virtual VK_Buffer* createBuffer(const std::vector<VK_Vertex>& vertices) = 0;
    virtual void addBuffer(VK_Buffer* buffer) = 0;
    virtual void removeBuffer(VK_Buffer* buffer) = 0;

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


