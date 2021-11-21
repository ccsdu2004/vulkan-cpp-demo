#include <iostream>
#include <chrono>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include "VK_UniformBuffer.h"
#include "VK_Context.h"

using namespace std;

const std::vector<VK_Vertex> vertices = {
    {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.5f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2
};

VK_Context* context = nullptr;

uint32_t updateUniformBufferData(char* & data, uint32_t size)
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    memcpy(data, &model[0][0], size);
    return sizeof(model);
}

int main()
{
    VK_ContextConfig config;
    config.debug = true;
    config.name = "Uniform Demo";

    context = createVkContext(config);
    context->createWindow(640, 480, true);

    VK_Context::VK_Config vkConfig;
    context->initVulkan(vkConfig);

    auto shaderSet = context->createShaderSet();
    shaderSet->addShader("shader/mvp/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderSet->addShader("shader/mvp/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    if(!shaderSet->isValid()) {
        std::cerr << "invalid shaderSet" << std::endl;
        shaderSet->release();
        context->release();
        return -1;
    }

    context->initPipeline(shaderSet);

    auto buffer = context->createVertexBuffer(vertices, indices);
    context->addBuffer(buffer);

    auto ubo = context->createUniformBuffer(sizeof(GLfloat) * 16);
    ubo->setWriteDataCallback(updateUniformBufferData);
    context->setUniformBuffer(ubo);

    context->createCommandBuffers();

    context->run();
    context->release();

    return 0;
}

