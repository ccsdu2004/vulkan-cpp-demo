#include <iostream>
#include "VK_Context.h"

using namespace std;

const std::vector<float> vertices1 = {
    0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f,
    0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f,
    -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f
};

const std::vector<float> vertices2 = {
    -0.5f, -0.3f, 0.0f, 1.0f, 1.0f, 1.0f, 0.5f,
        0.5f, -0.3f, 0.0f, 1.0f, 1.0f, 1.0f, 0.5f,
        0.5f, 0.3f, 0.0f, 1.0f, 1.0f, 1.0f, 0.5f,
        0.5f, 0.3f, 0.0f, 1.0f, 1.0f, 1.0f, 0.5f,
        -0.5f, 0.3f, 0.0f, 1.0f, 1.0f, 1.0f, 0.5f,
        -0.5f, -0.3f, 0.0f, 1.0f, 1.0f, 1.0f, 0.5f
    };

VK_Context* context = nullptr;

void onMouseButtonCallback(int button, int action, int mods)
{
    auto blend = context->getColorBlendAttachmentState();
    if(action) {
        blend.blendEnable = VK_TRUE;
        blend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blend.colorBlendOp = VK_BLEND_OP_ADD;
        blend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blend.alphaBlendOp = VK_BLEND_OP_ADD;
        context->setColorBlendAttachmentState(blend);
        context->setClearColor(0, 0, 0.2, 0.3);
    } else {
        blend.blendEnable = VK_FALSE;
        context->setColorBlendAttachmentState(blend);
        context->setClearColor(0, 0.2, 0.2, 0.3);
    }
}

int main()
{
    VK_ContextConfig config;
    config.debug = false;
    config.mouseCallback = &onMouseButtonCallback;

    context = createVkContext(config);
    context->createWindow(640, 480, true);

    VK_Context::VK_Config vkConfig;
    context->initVulkanDevice(vkConfig);

    auto shaderSet = context->createShaderSet();
    shaderSet->addShader("../shader/cube-color/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderSet->addShader("../shader/cube-color/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

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

    buffer = context->createVertexBuffer(vertices2, 7);
    context->addBuffer(buffer);

    context->createCommandBuffers();

    context->run();
    context->release();

    return 0;
}
