#include <iostream>
#include <cstring>
#include <chrono>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include "VK_UniformBuffer.h"
#include "VK_Context.h"
#include "VK_Image.h"
#include "VK_Texture.h"
#include "VK_DynamicState.h"
#include "VK_Pipeline.h"

using namespace std;

VK_Context *context = nullptr;
VK_Pipeline *pipeline = nullptr;

uint32_t updateUniformBufferData(char *&data, uint32_t size)
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>
                 (currentTime - startTime).count();
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model *= glm::rotate(glm::mat4(1.0f), time * glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    auto view = glm::lookAt(glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,
                            0.0f, 1.0f));
    auto proj = glm::perspective(glm::radians(45.0f),
                                 context->getSwapChainExtent().width / (float)context->getSwapChainExtent().height, 0.1f, 10.0f);
    proj[1][1] *= -1;

    model = proj * view * model;

    memcpy(data, &model[0][0], size);

    time = sin(time);

    memcpy(data + sizeof(float) * 16, (void *)&time, sizeof(float));

    return 17 * sizeof(float);
}

void onFrameSizeChanged(int width, int height)
{
    pipeline->getDynamicState()->applyDynamicViewport({0, 0, (float)width, (float)height, 0, 1});
}

#define INSTANCE_COUNT 7

int main()
{
    VK_ContextConfig config;
    config.debug = true;
    config.name = "Model IndirectDraw";

    context = createVkContext(config);
    context->createWindow(480, 480, true);
    context->setOnFrameSizeChanged(onFrameSizeChanged);

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.multiDrawIndirect = VK_TRUE;
    deviceFeatures.fillModeNonSolid = VK_TRUE;
    deviceFeatures.drawIndirectFirstInstance = VK_TRUE;
    context->setLogicalDeviceFeatures(deviceFeatures);

    VK_Context::VK_Config vkConfig;
    context->initVulkanDevice(vkConfig);

    auto shaderSet = context->createShaderSet();
    shaderSet->addShader("../shader/model-instance/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderSet->addShader("../shader/model-instance/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    shaderSet->appendVertexAttributeDescription(0, sizeof (float) * 3, VK_FORMAT_R32G32B32_SFLOAT, 0, 0);
    shaderSet->appendVertexAttributeDescription(1, sizeof (float) * 2, VK_FORMAT_R32G32_SFLOAT,
            sizeof(float) * 3, 0);
    shaderSet->appendVertexAttributeDescription(2, sizeof (float) * 3, VK_FORMAT_R32G32B32_SFLOAT,
            sizeof(float) * 5, 0);

    shaderSet->appendVertexAttributeDescription(3, sizeof(glm::vec3), VK_FORMAT_R32G32B32_SFLOAT,
            0, 1);

    shaderSet->appendVertexInputBindingDescription(8 * sizeof(float), 0, VK_VERTEX_INPUT_RATE_VERTEX);
    shaderSet->appendVertexInputBindingDescription(sizeof(glm::vec3), 1, VK_VERTEX_INPUT_RATE_INSTANCE);

    VkDescriptorSetLayoutBinding uniformBinding = VK_ShaderSet::createDescriptorSetLayoutBinding(0,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    shaderSet->addDescriptorSetLayoutBinding(uniformBinding);

    if (!shaderSet->isValid()) {
        std::cerr << "invalid shaderSet" << std::endl;
        shaderSet->release();
        context->release();
        return -1;
    }

    auto ubo = shaderSet->addUniformBuffer(0, sizeof(float) * 17);
    ubo->setWriteDataCallback(updateUniformBufferData);

    context->initVulkanContext();

    pipeline = context->createPipeline(shaderSet);

    auto rasterCreateInfo = pipeline->getRasterizationStateCreateInfo();
    rasterCreateInfo.polygonMode = VK_POLYGON_MODE_LINE;
    pipeline->setRasterizationStateCreateInfo(rasterCreateInfo);

    pipeline->create();

    std::vector<glm::vec3> instance(INSTANCE_COUNT);

    for (uint32_t i = 0; i < INSTANCE_COUNT; i++) {
        if (i > 0)
            instance[i].x = (i % 2 == 1) ? 0.5f * (i + 1) / 2 : -0.5f * (i + 1) / 2;
    }

    auto buffer = context->createVertexBuffer("../model/pug.obj", true, true);
    pipeline->addRenderBuffer(buffer);

    uint32_t count = buffer->getDataCount();

    buffer = context->createInstanceBuffer(INSTANCE_COUNT, sizeof(glm::vec3),
                                           (char *)instance.data(), 1);
    pipeline->addRenderBuffer(buffer);

    buffer = context->createIndirectBuffer(INSTANCE_COUNT, sizeof(glm::vec3), count);
    pipeline->addRenderBuffer(buffer);

    context->createCommandBuffers();

    context->run();
    context->release();

    return 0;
}
