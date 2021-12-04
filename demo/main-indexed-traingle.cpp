#include <iostream>
#include "VK_Context.h"

using namespace std;

const std::vector<float> vertices = {
    0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f,
    0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f,
    -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.5f,
    -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.5f
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 1
};

VK_Context* context = nullptr;

void onMouseButtonCallback(int button, int action, int mods)
{
    auto blend = context->getColorBlendAttachmentState();
    if(button) {
        blend.blendEnable = VK_TRUE;
        blend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blend.colorBlendOp = VK_BLEND_OP_ADD;
        blend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blend.alphaBlendOp = VK_BLEND_OP_ADD;
        context->setColorBlendAttachmentState(blend);
    } else {
        blend.blendEnable = VK_FALSE;
        context->setColorBlendAttachmentState(blend);
    }
}

int main()
{
    VK_ContextConfig config;
    config.debug = true;
    config.name = "Indexed-Triangle";
    config.mouseCallback = &onMouseButtonCallback;

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

    auto buffer = context->createVertexBuffer(vertices, 12, indices);
    context->addBuffer(buffer);

    context->createCommandBuffers();

    context->run();
    context->release();

    return 0;
}
