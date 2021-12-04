#ifndef VERTEX_H
#define VERTEX_H
#include <array>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct VK_Vertex {
    glm::vec3 position;
    glm::vec4 color;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
};

#endif // VERTEX_H
