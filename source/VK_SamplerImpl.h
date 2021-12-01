#ifndef VK_SAMPLERIMPL_H
#define VK_SAMPLERIMPL_H
#include "VK_Texture.h"
#include "VK_ContextImpl.h"

class VK_SamplerImpl : public VK_Sampler
{
public:
    VK_SamplerImpl(VkDevice vkDevice, VK_ContextImpl* vkContext);
    ~VK_SamplerImpl();
public:
    bool create(const VkSamplerCreateInfo& samplerInfo);
    void release()override;

    VkSampler getSampler()const override;
private:
    VkDevice device = nullptr;
    VK_ContextImpl* context = nullptr;
    VkSampler textureSampler = 0;
};

#endif
