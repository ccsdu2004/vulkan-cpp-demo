#include <iostream>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include "VK_UniformBuffer.h"
#include "VK_DescriptorSetLayoutBindingGroup.h"
#include "VK_Context.h"
#include "VK_Image.h"
#include "VK_Texture.h"

using namespace std;

const std::vector<float> vertices = {
    -0.5f, -0.5, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f
    };

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

VK_Context* context = nullptr;

uint32_t updateUniformBufferData(char* & data, uint32_t size)
{
    glm::mat4 model = glm::identity<glm::mat4>();
    memcpy(data, &model[0][0], size);
    return sizeof(model);
}

void onFrameSizeChanged(int width, int height)
{
    auto vp = VK_Viewports::createViewport(width, height);
    VK_Viewports vps;
    vps.addViewport(vp);
    context->setViewports(vps);
}

int main()
{
    VK_ContextConfig config;
    config.debug = true;
    config.name = "Texure Demo";

    context = createVkContext(config);
    context->createWindow(480, 480, true);
    context->setOnFrameSizeChanged(onFrameSizeChanged);

    VK_Context::VK_Config vkConfig;
    context->initVulkanDevice(vkConfig);

    auto shaderSet = context->createShaderSet();
    shaderSet->addShader("shader/texture/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderSet->addShader("shader/texture/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    shaderSet->appendAttributeDescription(0, sizeof (float) * 3);
    shaderSet->appendAttributeDescription(1, sizeof (float) * 4);
    shaderSet->appendAttributeDescription(2, sizeof (float) * 2);

    if(!shaderSet->isValid()) {
        std::cerr << "invalid shaderSet" << std::endl;
        shaderSet->release();
        context->release();
        return -1;
    }

    {
        VK_DescriptorSetLayoutBindingGroup bindingGroup;
        VkDescriptorSetLayoutBinding uniformBinding = VK_DescriptorSetLayoutBindingGroup::createDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
        bindingGroup.addDescriptorSetLayoutBinding(uniformBinding);
        uniformBinding = VK_DescriptorSetLayoutBindingGroup::createDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
        bindingGroup.addDescriptorSetLayoutBinding(uniformBinding);
        context->setDescriptorSetLayoutBindingGroup(bindingGroup);
    }

    {
        auto desciptorPoolSizeGroup = context->getDescriptorPoolSizeGroup();
        desciptorPoolSizeGroup.addDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        desciptorPoolSizeGroup.addDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        context->setDescriptorPoolSizeGroup(desciptorPoolSizeGroup);
    }

    auto buffer = context->createVertexBuffer(vertices, 9, indices);
    context->addBuffer(buffer);

    auto ubo = context->createUniformBuffer(0, sizeof(GLfloat) * 16);
    ubo->setWriteDataCallback(updateUniformBufferData);
    context->addUniformBuffer(ubo);

    auto image = context->createImage("images/smile.png");
    auto imageViewCreateInfo = VK_Texture::createImageViewCreateInfo(image->getImage(), VK_FORMAT_R8G8B8A8_SRGB);
    auto samplerCreateInfo  = VK_Texture::createSamplerCreateInfo();
    auto texture = context->createTexture(imageViewCreateInfo, samplerCreateInfo);

    context->initVulkanContext();
    context->initPipeline(shaderSet);
    context->createCommandBuffers();

    context->run();
    context->release();

    return 0;
}


