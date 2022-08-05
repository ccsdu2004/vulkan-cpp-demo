#include <iostream>
#include <cstring>
#include <chrono>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include "VK_UniformBuffer.h"
#include "VK_Context.h"
#include "VK_Pipeline.h"
#include "VK_DynamicState.h"

using namespace std;

const std::vector<float> vertices = {
    0.0f, 0.8f, 0.0f, 0.0f, 0.5f, 0.0f, 0.5f,
    0.05f, -0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 0.5f,
    -0.05f, -0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f
};

const std::vector<uint32_t> indices = {
    0, 1, 2
};

VK_Context *context = nullptr;
VK_Pipeline *pipeline = nullptr;
uint32_t uboSize = 0;

uint32_t uboCount = 36;
uint32_t speed = 13.0;

uint32_t updateUniformBufferData(char *&data, uint32_t size)
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>
                 (currentTime - startTime).count();

    for (int i = 0; i < uboCount; i++) {
        glm::mat4 model = glm::rotate(glm::mat4(1.0f),
                                      speed * time * glm::radians(90.0f) + i * glm::radians(18.0f),
                                      glm::vec3(0.0f, 0.0f,
                                                1.0f));
        memcpy(data + uboSize * i, &model[0][0], uboSize);
    }

    return uboCount * uboSize;
}

void onFrameSizeChanged(int width, int height)
{
    pipeline->getDynamicState()->applyDynamicViewport({0, 0, (float)width, (float)height, 0, 1});
}

int main()
{
    VK_ContextConfig config;
    config.debug = true;
    config.name = "Dynamic Uniform";

    context = createVkContext(config);
    context->createWindow(640, 480, true);
    context->setOnFrameSizeChanged(onFrameSizeChanged);

    VK_Context::VK_Config vkConfig;
    context->initVulkanDevice(vkConfig);

    uboSize = sizeof(GLfloat) * 16;

    auto property = context->getPhysicalDeviceProperties();
    if (property.limits.minUniformBufferOffsetAlignment)
        uboSize = (uboSize + property.limits.minUniformBufferOffsetAlignment - 1) &
                  ~(property.limits.minUniformBufferOffsetAlignment - 1);

    auto shaderSet = context->createShaderSet();
    shaderSet->addShader("../shader/dynamic-uniform/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderSet->addShader("../shader/dynamic-uniform/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    shaderSet->appendVertexAttributeDescription(0, sizeof (float) * 3, VK_FORMAT_R32G32B32_SFLOAT, 0);
    shaderSet->appendVertexAttributeDescription(1, sizeof (float) * 4, VK_FORMAT_R32G32B32A32_SFLOAT,
                                                sizeof(float) * 3);

    shaderSet->appendVertexInputBindingDescription(7 * sizeof(float), 0, VK_VERTEX_INPUT_RATE_VERTEX);

    VkDescriptorSetLayoutBinding uniformBinding = VK_ShaderSet::createDescriptorSetLayoutBinding(0,
                                                                                                 VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT);
    shaderSet->addDescriptorSetLayoutBinding(uniformBinding);

    if (!shaderSet->isValid()) {
        std::cerr << "invalid shaderSet" << std::endl;
        shaderSet->release();
        context->release();
        return -1;
    }

    auto ubo = shaderSet->addDynamicUniformBuffer(0, uboSize, uboCount);
    ubo->setWriteDataCallback(updateUniformBufferData);

    context->initVulkanContext();
    pipeline = context->createPipeline(shaderSet);
    pipeline->getDynamicState()->addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
    pipeline->create();
    pipeline->getDynamicState()->applyDynamicViewport({0, 0, 640, 480, 0, 1});
    auto buffer = context->createVertexBuffer(vertices, 3 + 4, indices);
    pipeline->addRenderBuffer(buffer);

    context->createCommandBuffers();

    context->run();
    context->release();

    return 0;
}

