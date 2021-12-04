#include <iostream>
#include "VK_Context.h"

using namespace std;

const std::vector<float> vertices1 = {
    0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
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

    auto buffer = context->createVertexBuffer(vertices1, 7);
    context->addBuffer(buffer);

    context->createCommandBuffers();

    context->run();
    context->release();

    return 0;
}
