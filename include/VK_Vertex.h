#ifndef VERTEX_H
#define VERTEX_H
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <array>
#include <vmath.h>

struct VK_Vertex
{
    vmath::vec3 position;
    vmath::vec4 color;
    vmath::vec3 normal;
    vmath::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions();
};

#endif // VERTEX_H
