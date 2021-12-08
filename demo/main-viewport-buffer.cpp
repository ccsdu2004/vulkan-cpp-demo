#include <iostream>
#include <chrono>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include "VK_UniformBuffer.h"
#include "VK_Context.h"

using namespace std;

const std::vector<VK_Vertex> vertices = {
    {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.5f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.5f}},
    {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.5f}},
    {{0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 0.5f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 1, 2, 3
};

VK_Context* context = nullptr;

VK_Viewports viewports;
VK_Viewports newViewports;

void onMouseButtonCallback(int button, int action, int mods)
{
    auto size = context->getSwapChainExtent();

    auto vp1 = VK_Viewports::createViewport(size.width >> 1, size.height);
    auto vp2 = VK_Viewports::createViewport(size.width >> 1, size.height);

    newViewports.clear();

    newViewports.addViewport(vp1);
    vp2.x = vp1.width;
    newViewports.addViewport(vp2);

    auto scissor2 = VK_Viewports::createScissor(size.width >> 1, size.height);
    scissor2.offset.x = size.width * 0.25f;
    newViewports.setScissor(0, scissor2);

    if(action) {
        context->setViewports(viewports);
    } else {
        context->setViewports(newViewports);
    }
}

void onFrameSizeChanged(int width, int height)
{
    auto vp = VK_Viewports::createViewport(width, height);
    VK_Viewports vps;
    vps.addViewport(vp);
    context->setViewports(vps);
}

int main()
{
    VK_ContextConfig config;
    config.debug = true;
    config.name = "Viewport Demo";
    config.mouseCallback = &onMouseButtonCallback;

    context = createVkContext(config);
    context->createWindow(640, 480, true);
    context->setOnFrameSizeChanged(onFrameSizeChanged);

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.multiViewport = VK_TRUE;
    context->setLogicalDeviceFeatures(deviceFeatures);

    VK_Context::VK_Config vkConfig;
    context->initVulkanDevice(vkConfig);

    auto shaderSet = context->createShaderSet();
    shaderSet->addShader("../shader/vertex/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderSet->addShader("../shader/vertex/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    shaderSet->appendAttributeDescription(0, sizeof (float) * 3);
    shaderSet->appendAttributeDescription(1, sizeof (float) * 4);

    if(!shaderSet->isValid()) {
        std::cerr << "invalid shaderSet" << std::endl;
        shaderSet->release();
        context->release();
        return -1;
    }

    context->initVulkanContext(shaderSet);
    context->initPipeline();

    viewports = context->getViewports();

    auto buffer = context->createVertexBuffer(vertices, indices);
    context->addBuffer(buffer);

    context->createCommandBuffers();

    context->run();
    context->release();

    return 0;
}

