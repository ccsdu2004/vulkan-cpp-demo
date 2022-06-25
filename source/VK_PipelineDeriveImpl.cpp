#include <iostream>
#include <cassert>
#include "VK_PipelineDeriveImpl.h"
#include "VK_ContextImpl.h"

VK_PipelineDeriveImpl::VK_PipelineDeriveImpl(VK_ContextImpl *context, VK_ShaderSet *shader,
                                             VK_PipelineImpl *inputParent):
    VK_PipelineImpl(context, shader, inputParent)
{
}

bool VK_PipelineDeriveImpl::create()
{
    return createPipeline(VkPipelineCreateFlagBits(0));
}
