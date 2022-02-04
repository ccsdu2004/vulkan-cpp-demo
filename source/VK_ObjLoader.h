#ifndef VK_OBJLOADER_H
#define VK_OBJLOADER_H
#include <vector>
#include <string>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "VK_VertexBuffer.h"

class VK_OBJLoader : public VK_VertexBuffer
{
public:
    VK_OBJLoader() = delete;
    VK_OBJLoader(VK_Context *context):
        VK_VertexBuffer(context)
    {
    }

    bool load(const std::string &file, bool zero = true);
    glm::vec3 getMinPosition()const;
    glm::vec3 getMaxPosition()const;

    std::vector<std::vector<float>> getData()const
    {
        return data;
    }
private:
    glm::vec3 minPosition;
    glm::vec3 maxPosition;
    std::vector<std::vector<float>> data;
};

#endif // MODELLOADER_H
