#include <vector>
#include <numeric>
#include <iostream>
#include "VK_ShaderSetImpl.h"
#include "VK_Util.h"

VK_ShaderSetImpl::VK_ShaderSetImpl(VkDevice device):
    vkDevice(device)
{
}

VK_ShaderSetImpl::~VK_ShaderSetImpl()
{
}

void VK_ShaderSetImpl::release()
{
    for(auto itr = shaderStageCreateInfos.begin(); itr != shaderStageCreateInfos.end(); itr++)
        vkDestroyShaderModule(vkDevice, itr->module, nullptr);
    delete this;
}

void VK_ShaderSetImpl::appendAttributeDescription(int index, int size)
{
    if(index == 0)
        sizeTable.clear();

    if(sizeTable.size() != (size_t)index)
        return;

    if(size != 2 * sizeof(float) &&
       size != 3 * sizeof(float) &&
       size != 4 * sizeof(float))
        return;

    sizeTable.push_back(size);
    vertexInputAttributeDescriptions.resize(sizeTable.size());

    auto fn = [](int size)->VkFormat {
        if(size == 2 * sizeof (float))
            return VK_FORMAT_R32G32_SFLOAT;
        else if(size == 3 * sizeof (float))
            return VK_FORMAT_R32G32B32_SFLOAT;
        else if(size == 4 * sizeof (float))
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    };

    int offset = 0;
    for(size_t i = 0; i < sizeTable.size(); i++) {
        vertexInputAttributeDescriptions[i].binding = 0;
        vertexInputAttributeDescriptions[i].location = i;
        vertexInputAttributeDescriptions[i].format = fn(sizeTable[i]);
        vertexInputAttributeDescriptions[i].offset = offset;
        offset += sizeTable[i];
    }
}

VkVertexInputBindingDescription* VK_ShaderSetImpl::getBindingDescription()
{
    bindingDescription.binding = 0;
    bindingDescription.stride = std::accumulate(sizeTable.begin(), sizeTable.end(), 0);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return &bindingDescription;
}

size_t VK_ShaderSetImpl::getAttributeDescriptionCount()const
{
    return vertexInputAttributeDescriptions.size();
}

const VkVertexInputAttributeDescription* VK_ShaderSetImpl::getAttributeDescriptionData()const
{
    return vertexInputAttributeDescriptions.data();
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
    while(itr != descriptorPoolSizes.end()) {
        (*itr).descriptorCount = size;
        itr ++;
    }
}

bool VK_ShaderSetImpl::addShader(const std::string &spvFile, VkShaderStageFlagBits type, const char* entryPoint)
{
    auto module = createShaderModule(spvFile);
    if(!module)
        return false;

    auto itr = shaderStageCreateInfos.begin();
    while(itr != shaderStageCreateInfos.end()) {
        if(itr->flags == type)
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

bool VK_ShaderSetImpl::isValid()
{
    //待完善
    bool hasV = false;
    bool hasFs = false;
    auto itr = shaderStageCreateInfos.begin();
    while(itr != shaderStageCreateInfos.end()) {
        if(itr->stage == VK_SHADER_STAGE_VERTEX_BIT)
            hasV = true;
        else if(itr->stage == VK_SHADER_STAGE_FRAGMENT_BIT)
            hasFs = true;
        itr ++;
    }
    return hasV && hasFs;
}

VkPipelineShaderStageCreateInfo* VK_ShaderSetImpl::getCreateInfoData()
{
    return shaderStageCreateInfos.data();
}

size_t VK_ShaderSetImpl::getCreateInfoCount()
{
    return shaderStageCreateInfos.size();
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

    if (vkCreateShaderModule(vkDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        std::cerr << "failed to create shader module!" << std::endl;

    return shaderModule;
}
