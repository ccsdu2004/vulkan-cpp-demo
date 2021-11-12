#include <iostream>
#include "VK_Context.h"

using namespace std;

const std::vector<VK_Vertex> vertices1 = {
    {{0.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}
};

VK_Context* context = nullptr;

int main()
{
    VK_ContextConfig config;
    config.name = "Hello-Triangle";
    config.debug = false;

    context = createVkContext(config);
    context->createWindow(640, 480, true);

    VK_Context::VK_Config vkConfig;
    context->initVulkan(vkConfig);

    auto shaderSet = context->createShaderSet();
    shaderSet->addShader("shader/vertex/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderSet->addShader("shader/vertex/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    if(!shaderSet->isValid()) {
        std::cerr << "invalid shaderSet" << std::endl;
        shaderSet->release();
        context->release();
        return -1;
    }

    context->initPipeline(shaderSet);

    auto buffer = context->createVertexBuffer(vertices1);
    context->addBuffer(buffer);

    context->createCommandBuffers();

    context->run();
    context->release();

    return 0;
}
