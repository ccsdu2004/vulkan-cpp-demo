#include <iostream>
#include <cstring>
#include <chrono>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include "VK_UniformBuffer.h"
#include "VK_Context.h"
#include "VK_Image.h"
#include "VK_Texture.h"
#include "VK_Pipeline.h"
#include "VK_DynamicState.h"

using namespace std;

VK_Context *context = nullptr;
VK_Pipeline *pipeline = nullptr;

struct UBO {
    glm::mat4 projection;
    glm::mat4 model;
    glm::vec4 lightPos = glm::vec4(0.2f, 0.5f, 1.2f, 0.0f);
};

uint32_t updateUniformBufferData(char *&data, uint32_t size)
{
    assert(sizeof(UBO) == size);
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>
                 (currentTime - startTime).count();
    glm::mat4 model = glm::identity<glm::mat4>();
    model *= glm::scale(glm::vec3(1.5f, 1.5f, 1.5f));
    model *= glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model *= glm::rotate(glm::mat4(1.0f), time * glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    auto view = glm::lookAt(glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,
                            0.0f, 1.0f));
    auto proj = glm::perspective(glm::radians(45.0f),
                                 context->getSwapChainExtent().width / (float)context->getSwapChainExtent().height, 0.1f, 10.0f);
    proj[1][1] *= -1;

    UBO ubo;
    ubo.projection = proj * view;
    ubo.model = model;
    memcpy(data, &ubo, sizeof(UBO));
    time = sin(time);
    return sizeof(UBO);
}

uint32_t updateUniformBufferData2(char *&data, uint32_t size)
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>
                 (currentTime - startTime).count();
    time = (std::cos(time) + 1.0) * 0.5;
    memcpy(data, &time, size);
    return size;
}

void onFrameSizeChanged(int width, int height)
{
    pipeline->getDynamicState()->applyDynamicViewport({0, 0, (float)width, (float)height, 0, 1});
}

int main()
{
    VK_ContextConfig config;
    config.debug = false;
    config.name = "Model Cartoon";

    context = createVkContext(config);
    context->createWindow(480, 480, true);
    context->setOnFrameSizeChanged(onFrameSizeChanged);

    VK_Context::VK_Config vkConfig;
    context->initVulkanDevice(vkConfig);

    auto shaderSet = context->createShaderSet();
    shaderSet->addShader("../shader/model-toon/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderSet->addShader("../shader/model-toon/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    shaderSet->appendVertexAttributeDescription(0, sizeof (float) * 3, VK_FORMAT_R32G32B32_SFLOAT, 0);
    shaderSet->appendVertexAttributeDescription(1, sizeof (float) * 2, VK_FORMAT_R32G32_SFLOAT,
            sizeof(float) * 3);
    shaderSet->appendVertexAttributeDescription(2, sizeof (float) * 3, VK_FORMAT_R32G32B32_SFLOAT,
            sizeof(float) * 5);

    shaderSet->appendVertexInputBindingDescription(8 * sizeof(float), 0, VK_VERTEX_INPUT_RATE_VERTEX);

    VkDescriptorSetLayoutBinding uniformBinding = VK_ShaderSet::createDescriptorSetLayoutBinding(0,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    shaderSet->addDescriptorSetLayoutBinding(uniformBinding);

    auto samplerBinding = VK_ShaderSet::createDescriptorSetLayoutBinding(1,
                          VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    shaderSet->addDescriptorSetLayoutBinding(uniformBinding);
    auto samplerCreateInfo  = VK_Sampler::createSamplerCreateInfo();
    auto samplerPtr = context->createSampler(samplerCreateInfo);
    VkSampler sampler = samplerPtr->getSampler();
    samplerBinding.pImmutableSamplers = &sampler;

    shaderSet->addDescriptorSetLayoutBinding(samplerBinding);

    auto uniformBinding2 = VK_ShaderSet::createDescriptorSetLayoutBinding(2,
                           VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
    shaderSet->addDescriptorSetLayoutBinding(uniformBinding2);

    if (!shaderSet->isValid()) {
        std::cerr << "invalid shaderSet" << std::endl;
        shaderSet->release();
        context->release();
        return -1;
    }

    auto ubo = shaderSet->addUniformBuffer(0, sizeof(UBO));
    ubo->setWriteDataCallback(updateUniformBufferData);

    auto ubo2 = shaderSet->addUniformBuffer(2, sizeof(float));
    ubo2->setWriteDataCallback(updateUniformBufferData2);

    auto image = context->createImage("../model/PUG_TAN.tga");

    auto imageViewCreateInfo = VK_ImageView::createImageViewCreateInfo(image->getImage(),
                               VK_FORMAT_R8G8B8A8_SRGB);
    auto imageView = context->createImageView(imageViewCreateInfo);
    shaderSet->addImageView(imageView);

    context->initVulkanContext();

    pipeline = context->createPipeline(shaderSet);

    auto rasterCreateInfo = pipeline->getRasterizationStateCreateInfo();
    rasterCreateInfo.cullMode = VK_CULL_MODE_NONE;
    pipeline->setRasterizationStateCreateInfo(rasterCreateInfo);

    auto depthStencilState = pipeline->getDepthStencilStateCreateInfo();
    depthStencilState.stencilTestEnable = VK_TRUE;
    depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depthStencilState.back.failOp = VK_STENCIL_OP_REPLACE;
    depthStencilState.back.depthFailOp = VK_STENCIL_OP_REPLACE;
    depthStencilState.back.passOp = VK_STENCIL_OP_REPLACE;
    depthStencilState.back.compareMask = 0xff;
    depthStencilState.back.writeMask = 0xff;
    depthStencilState.back.reference = 1;
    depthStencilState.front = depthStencilState.back;
    pipeline->setDepthStencilStateCreateInfo(depthStencilState);

    pipeline->create();
    auto buffer = context->createVertexBuffer("../model/pug.obj", true);
    pipeline->addRenderBuffer(buffer);

    context->createCommandBuffers();

    context->run();
    context->release();

    return 0;
}
