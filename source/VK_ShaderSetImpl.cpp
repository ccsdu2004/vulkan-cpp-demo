#include <vector>
#include <fstream>
#include <iostream>
#include "VK_ShaderSetImpl.h"

std::vector<char> readSpvFile(const std::string &filename)
{
    std::vector<char> buffer;

    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "failed to open file:" << filename.data() << std::endl;
        return buffer;
    }

    size_t size = (size_t) file.tellg();
    buffer.resize(size);

    file.seekg(0);
    file.read(buffer.data(), size);
    file.close();
    return buffer;
}

VK_ShaderSetImpl::VK_ShaderSetImpl(VkDevice device):
    vkDevice(device)
{
}

VK_ShaderSetImpl::~VK_ShaderSetImpl()
{
}

void VK_ShaderSetImpl::release()
{
    delete this;
}

bool VK_ShaderSetImpl::addShader(const std::string &spvFile, VkShaderStageFlagBits type, const std::string &entryPoint)
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
    createInfo.pName = entryPoint.data();
    createInfo.module = module;

    shaderStageCreateInfos.push_back(createInfo);
    return true;
}

bool VK_ShaderSetImpl::isValid()
{
    bool hasV = false, hasFs = false;
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

std::vector<VkPipelineShaderStageCreateInfo> VK_ShaderSetImpl::getCreateInfo()
{
    return shaderStageCreateInfos;
}

VkShaderModule VK_ShaderSetImpl::createShaderModule(const std::string &spvFile)
{
    VkShaderModule shaderModule = 0;
    auto code = readSpvFile(spvFile);
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
