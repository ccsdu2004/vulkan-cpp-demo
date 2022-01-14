#ifndef VK_PIPELINEDERIVEIMPL_H
#define VK_PIPELINEDERIVEIMPL_H
#include <VK_PipelineImpl.h>

class VK_PipelineDeriveImpl : public VK_PipelineImpl
{
public:
    VK_PipelineDeriveImpl() = delete;
    VK_PipelineDeriveImpl(VK_ContextImpl* context, VK_ShaderSet* shader, VK_PipelineImpl* inputParent = nullptr);
public:
    bool create() override final;
};

#endif // VK_PIPELINEDERIVEIMPL_H
