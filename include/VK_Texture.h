#ifndef VK_SAMPLER_H
#define VK_SAMPLER_H
#include <vulkan/vulkan.h>
#include "VK_Deleter.h"

class VK_Sampler : public VK_Deleter
{
public:
    static VkSamplerCreateInfo createSamplerCreateInfo();
    virtual VkSampler getSampler()const = 0;
};

#endif
