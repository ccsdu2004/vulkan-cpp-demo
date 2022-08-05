#include <vector>
#include <numeric>
#include <iostream>
#include <VK_ContextImpl.h>
#include <VK_ShaderSetImpl.h>
#include <VK_DescriptorSets.h>
#include <VK_UniformBufferImpl.h>
#include <VK_DynamicUniformBuffer.h>
#include <VK_TexelBuffer.h>
#include <VK_StorageBuffer.h>
#include <VK_StorageTexelBuffer.h>
#include <VK_Util.h>

VK_ShaderSetImpl::VK_ShaderSetImpl(VK_ContextImpl *vkContext):
    context(vkContext)
{
}

VK_ShaderSetImpl::~VK_ShaderSetImpl()
{
}

void VK_ShaderSetImpl::release()
{
    for (auto itr = shaderStageCreateInfos.begin(); itr != shaderStageCreateInfos.end(); itr++)
        vkDestroyShaderModule(context->getDevice(), itr->module, context->getAllocation());

    for (auto uniform : uniformBuffers)
        uniform->release();
    uniformBuffers.clear();
    delete this;
}

void VK_ShaderSetImpl::appendVertexAttributeDescription(uint32_t index, uint32_t size,
                                                        VkFormat format,
                                                        uint32_t offset, uint32_t binding)
{
    VkVertexInputAttributeDescription description;
    description.binding = binding;
    description.location = index;
    description.format = format;
    description.offset = offset;

    vertexInputAttributeDescriptions.push_back(description);
}

void VK_ShaderSetImpl::appendVertexInputBindingDescription(uint32_t stride, uint32_t binding,
                                                           VkVertexInputRate inputRate)
{
    vertexInputBindingDescriptions.push_back({binding, stride, inputRate});
}

size_t VK_ShaderSetImpl::getVertexAttributeDescriptionCount()const
{
    return vertexInputAttributeDescriptions.size();
}

const VkVertexInputAttributeDescription *VK_ShaderSetImpl::getVertexAttributeDescriptionData()const
{
    return vertexInputAttributeDescriptions.data();
}

size_t VK_ShaderSetImpl::getVertexInputBindingDescriptionCount() const
{
    return vertexInputBindingDescriptions.size();
}

const VkVertexInputBindingDescription *VK_ShaderSetImpl::getVertexInputBindingDescriptionData()
const
{
    return vertexInputBindingDescriptions.data();
}

void VK_ShaderSetImpl::addDescriptorSetLayoutBinding(const VkDescriptorSetLayoutBinding &binding)
{
    descriptorSetLayoutBindings.push_back(binding);
    VkDescriptorPoolSize poolSize;
    poolSize.descriptorCount = 0;
    poolSize.type = binding.descriptorType;
    descriptorPoolSizes.push_back(poolSize);
}

size_t VK_ShaderSetImpl::getDescriptorSetLayoutBindingCount() const
{
    return descriptorSetLayoutBindings.size();
}

const VkDescriptorSetLayoutBinding *VK_ShaderSetImpl::getDescriptorSetLayoutBindingData() const
{
    return descriptorSetLayoutBindings.data();
}

size_t VK_ShaderSetImpl::getDescriptorPoolSizeCount() const
{
    return descriptorPoolSizes.size();
}

const VkDescriptorPoolSize *VK_ShaderSetImpl::getDescriptorPoolSizeData() const
{
    return descriptorPoolSizes.data();
}

void VK_ShaderSetImpl::updateDescriptorPoolSize(int32_t size)
{
    auto itr = descriptorPoolSizes.begin();
    while (itr != descriptorPoolSizes.end()) {
        (*itr).descriptorCount = size;
        itr ++;
    }
}

bool VK_ShaderSetImpl::addShader(const std::string &spvFile, VkShaderStageFlagBits type,
                                 const char *entryPoint)
{
    auto module = createShaderModule(spvFile);
    if (!module)
        return false;

    auto itr = shaderStageCreateInfos.begin();
    while (itr != shaderStageCreateInfos.end()) {
        if (itr->flags == type)
            return false;
        itr ++;
    }

    VkPipelineShaderStageCreateInfo createInfo{};
    createInfo.stage = type;
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.pName = entryPoint;
    createInfo.module = module;

    shaderStageCreateInfos.push_back(createInfo);
    return true;
}

VK_UniformBuffer *VK_ShaderSetImpl::addUniformBuffer(uint32_t binding, uint32_t bufferSize)
{
    auto buffer = new VK_UniformBufferImpl(context, binding, bufferSize);
    buffer->initBuffer(context->getSwapImageCount());
    uniformBuffers.push_back(buffer);
    return buffer;
}

VK_UniformBuffer *VK_ShaderSetImpl::addDynamicUniformBuffer(uint32_t binding,
                                                            uint32_t bufferSize, uint32_t count)
{
    auto buffer = new VK_DynamicUniformBuffer(context, binding, bufferSize, count);
    buffer->initBuffer(context->getSwapImageCount());
    uniformBuffers.push_back(buffer);
    return buffer;
}

VK_UniformBuffer *VK_ShaderSetImpl::addTexelBuffer(uint32_t binding, uint32_t bufferSize)
{
    auto buffer = new VK_TexelBuffer(context, binding, bufferSize);
    buffer->initBuffer(context->getSwapImageCount());
    uniformBuffers.push_back(buffer);
    return buffer;
}

VK_UniformBuffer *VK_ShaderSetImpl::addStorageBuffer(uint32_t binding, uint32_t bufferSize)
{
    auto buffer = new VK_StorageBuffer(context, binding, bufferSize);
    buffer->initBuffer(context->getSwapImageCount());
    uniformBuffers.push_back(buffer);
    return buffer;
}

VK_UniformBuffer *VK_ShaderSetImpl::addStorageTexelBuffer(uint32_t binding, uint32_t bufferSize)
{
    auto buffer = new VK_StorageTexelBuffer(context, binding, bufferSize);
    buffer->initBuffer(context->getSwapImageCount());
    uniformBuffers.push_back(buffer);
    return buffer;
}

void VK_ShaderSetImpl::initUniformBuffer()
{
    for (auto uniform : uniformBuffers)
        uniform->initBuffer(context->getSwapImageCount());
}

void VK_ShaderSetImpl::clearUniformBuffer()
{
    for (auto uniformBuffer : uniformBuffers)
        uniformBuffer->clearBuffer();
}

void VK_ShaderSetImpl::addImageView(VK_ImageView *imageView, uint32_t binding)
{
    if (imageView) {
        auto find = imageViews.find(imageView);
        if (find != imageViews.end())
            return;
        imageViews.insert(std::make_pair(imageView, binding));
    }
}

bool VK_ShaderSetImpl::isValid()
{
    bool hasV = false;
    bool hasFs = false;
    auto itr = shaderStageCreateInfos.begin();
    while (itr != shaderStageCreateInfos.end()) {
        if (itr->stage == VK_SHADER_STAGE_VERTEX_BIT)
            hasV = true;
        else if (itr->stage == VK_SHADER_STAGE_FRAGMENT_BIT)
            hasFs = true;
        itr ++;
    }
    return hasV && hasFs;
}

VkPipelineShaderStageCreateInfo *VK_ShaderSetImpl::getCreateInfoData()
{
    return shaderStageCreateInfos.data();
}

size_t VK_ShaderSetImpl::getCreateInfoCount()
{
    return shaderStageCreateInfos.size();
}

void VK_ShaderSetImpl::updateDescriptorSet(std::shared_ptr<VK_DescriptorSets> descriptorSet)
{
    descriptorSet->update(uniformBuffers, imageViews);
}

void VK_ShaderSetImpl::update(uint32_t index)
{
    for (auto uniform : uniformBuffers)
        uniform->update(index);
}

VkShaderModule VK_ShaderSetImpl::createShaderModule(const std::string &spvFile)
{
    VkShaderModule shaderModule = 0;
    auto code = readDataFromFile(spvFile);
    if (code.empty())
        return shaderModule;

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    if (vkCreateShaderModule(context->getDevice(), &createInfo, context->getAllocation(),
                             &shaderModule) != VK_SUCCESS)
        std::cerr << "failed to create shader module!" << std::endl;

    return shaderModule;
}
