#ifndef VK_TEXTUREIMPL_H
#define VK_TEXTUREIMPL_H
#include "VK_Texture.h"
#include "VK_ContextImpl.h"

class VK_TextureImpl : public VK_Texture
{
public:
    VK_TextureImpl(VkDevice vkDevice, VK_ContextImpl* vkContext);
    ~VK_TextureImpl();
public:
    bool create(const VkImageViewCreateInfo& viewCreateInfo, const VkSamplerCreateInfo& samplerInfo);
    void release()override;
public:
    VkImageView getTextureImageView()const override;
private:
    VkDevice device = nullptr;
    VK_ContextImpl* context = nullptr;

    VkImageView textureImageView = 0;
    VkSampler textureSampler = 0;
};

#endif // VK_TEXTUREIMPL_H
