#ifndef VK_VERTEXBUFFER_H
#define VK_VERTEXBUFFER_H
#include "VK_Buffer.h"

class VK_VertexBuffer : public VK_Buffer
{
public:
    VK_VertexBuffer(VK_Context* context, VkDevice vkDevice);
    ~VK_VertexBuffer();
public:
    void create(const std::vector<VK_Vertex>& vertices, VK_Context* context);
    void render(VkCommandBuffer command);
private:
    int count = 0;
};

#endif // VK_VERTEXBUFFER_H
