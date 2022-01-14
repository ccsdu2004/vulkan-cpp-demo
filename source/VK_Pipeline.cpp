#include <iostream>
#include "VK_Pipeline.h"

VkPipelineTessellationStateCreateInfo VK_Pipeline::createPipelineTessellationStateCreateInfo(uint32_t patch)
{
    VkPipelineTessellationStateCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    createInfo.patchControlPoints = patch;
    createInfo.flags = 0;
    createInfo.pNext = nullptr;
    return createInfo;
}
