#include <iostream>
#include <chrono>
#include <cstring>
#include <random>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include "VK_UniformBuffer.h"
#include "VK_Context.h"
#include "VK_Pipeline.h"
#include "VK_DynamicState.h"

using namespace std;

const std::vector<float> vertices = {
    0.0f, -0.5f, 0.0f,
    0.5f, 0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f
};

const std::vector<uint32_t> indices = {
    0, 1, 2
};

VK_Context *context = nullptr;
VK_Pipeline *pipeline = nullptr;

uint32_t updateUniformBufferData(char *&data, uint32_t size)
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>
                 (currentTime - startTime).count();
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * glm::radians(30.0f), glm::vec3(0.0f, 0.0f,
                                  1.0f));
    memcpy(data, &model[0][0], size);
    return sizeof(model);
}

uint32_t updateStorageBufferData(char *&data, uint32_t size)
{
    float color[12] = {0};
    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 0.0f;
    color[3] = 0.0f;

    color[4] = 1.0f;
    color[5] = 0.0f;
    color[6] = 1.0f;
    color[7] = 0.0f;

    color[8] = 0.0f;
    color[9] = 1.0f;
    color[10] = 1.0f;
    color[11] = 0.0f;
    memcpy(data, color, size);
    return sizeof(float) * 12;
}

void onFrameSizeChanged(int width, int height)
{
    pipeline->getDynamicState()->applyDynamicViewport({0, 0, (float)width, (float)height, 0, 1});
}

void onMouseButtonCallback(int button, int action, int mods)
{
    (void)button;
    (void)mods;

    if (action)
        context->captureScreenShot();
}

int main()
{
    srand(time(0));

    VK_ContextConfig config;
    config.debug = true;
    config.name = "Storage Buffer";
    config.mouseCallback = onMouseButtonCallback;

    context = createVkContext(config);
    context->createWindow(480, 480, true);
    context->setOnFrameSizeChanged(onFrameSizeChanged);

    VK_Context::VK_Config vkConfig;
    context->initVulkanDevice(vkConfig);

    auto shaderSet = context->createShaderSet();
    shaderSet->addShader("../shader/storagebuffer/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderSet->addShader("../shader/storagebuffer/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    shaderSet->appendVertexAttributeDescription(0, sizeof (float) * 3, VK_FORMAT_R32G32B32_SFLOAT, 0);


    shaderSet->appendVertexInputBindingDescription(3 * sizeof(float), 0, VK_VERTEX_INPUT_RATE_VERTEX);

    VkDescriptorSetLayoutBinding uniformBinding = VK_ShaderSet::createDescriptorSetLayoutBinding(0,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    shaderSet->addDescriptorSetLayoutBinding(uniformBinding);

    VkDescriptorSetLayoutBinding ssboBinding = VK_ShaderSet::createDescriptorSetLayoutBinding(1,
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);

    shaderSet->addDescriptorSetLayoutBinding(ssboBinding);

    if (!shaderSet->isValid()) {
        std::cerr << "invalid shaderSet" << std::endl;
        shaderSet->release();
        context->release();
        return -1;
    }

    auto uniformBuffer = shaderSet->addUniformBuffer(0, sizeof(float) * 16);
    uniformBuffer->setWriteDataCallback(updateUniformBufferData);

    auto storageBuffer = shaderSet->addStorageBuffer(1, sizeof(float) * 12);
    storageBuffer->setWriteDataCallback(updateStorageBufferData);

    context->initVulkanContext();
    pipeline = context->createPipeline(shaderSet);
    pipeline->getDynamicState()->addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
    pipeline->create();
    pipeline->getDynamicState()->applyDynamicViewport({0, 0, 480, 480, 0, 1});

    auto buffer = context->createVertexBuffer(vertices, 3, indices);
    pipeline->addRenderBuffer(buffer);

    context->createCommandBuffers();

    context->run();
    context->release();

    return 0;
}

