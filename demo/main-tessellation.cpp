#include <iostream>
#include "VK_Context.h"

using namespace std;

const std::vector<float> vertices1 = {
    0.0f, -0.75f, 0.0f,
    0.6f, 0.75f, 0.0f,
    -0.6f, 0.75f, 0.0f
};

VK_Context* context = nullptr;

int main()
{
    VK_ContextConfig config;
    config.name = "Tessellation";
    config.debug = true;

    context = createVkContext(config);
    context->createWindow(640, 480, true);

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.fillModeNonSolid = VK_TRUE;
    deviceFeatures.tessellationShader = VK_TRUE;
    context->setLogicalDeviceFeatures(deviceFeatures);

    VK_Context::VK_Config vkConfig;
    context->initVulkanDevice(vkConfig);

    auto shaderSet = context->createShaderSet();
    shaderSet->addShader("../shader/tess/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderSet->addShader("../shader/tess/tesc.spv", VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
    shaderSet->addShader("../shader/tess/tese.spv", VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
    shaderSet->addShader("../shader/tess/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    shaderSet->appendAttributeDescription(0, sizeof (float) * 3);

    if(!shaderSet->isValid()) {
        std::cerr << "invalid shaderSet" << std::endl;
        shaderSet->release();
        context->release();
        return -1;
    }

    context->initVulkanContext(shaderSet);

    auto tessellationCreateInfo = context->createPipelineTessellationStateCreateInfo();
    tessellationCreateInfo.patchControlPoints = 32;
    context->setPipelineTessellationStateCreateInfo(tessellationCreateInfo);

    auto rasterCreateInfo = context->getPipelineRasterizationStateCreateInfo();
    rasterCreateInfo.polygonMode = VK_POLYGON_MODE_LINE;
    rasterCreateInfo.lineWidth = 1.0f;
    context->setPipelineRasterizationStateCreateInfo(rasterCreateInfo);

    context->initPipeline();

    auto buffer = context->createVertexBuffer(vertices1, 3);
    context->addBuffer(buffer);

    context->createCommandBuffers();

    context->run();
    context->release();

    return 0;
}
