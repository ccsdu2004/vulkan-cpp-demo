#include "tiny_obj_loader.h"
#include "VK_ObjLoader.h"

inline glm::vec3 getMinPos(const glm::vec3 &left, const glm::vec3 &right)
{
    return glm::vec3(std::min(left.x, right.x), std::min(left.y, right.y), std::min(left.z, right.z));
}

inline glm::vec3 getMaxPos(const glm::vec3 &left, const glm::vec3 &right)
{
    return glm::vec3(std::max(left.x, right.x), std::max(left.y, right.y), std::max(left.z, right.z));
}

bool VK_OBJLoader::load(const std::string &file, bool zero)
{
    glm::vec3 minPosition(std::numeric_limits<float>::max());
    glm::vec3 maxPosition(std::numeric_limits<float>::min());

    data.clear();

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ok = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file.data(), nullptr, true, true);
    if (!ok)
        return false;

    for (size_t i = 0; i < shapes.size(); i++) {
        assert(shapes[i].mesh.num_face_vertices.size() == shapes[i].mesh.material_ids.size());
        assert(shapes[i].mesh.num_face_vertices.size() == shapes[i].mesh.smoothing_group_ids.size());

        size_t index_offset = 0;
        std::vector<float> items;

        for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
            size_t fnum = shapes[i].mesh.num_face_vertices[f];

            tinyobj::index_t idx;
            int vertex_index[3];
            int normal_index[3];
            int texcoord_index[3];

            for (size_t v = 0; v < fnum; v++) {
                idx = shapes[i].mesh.indices[index_offset + v];
                vertex_index[v] = idx.vertex_index;
                texcoord_index[v] = idx.texcoord_index;
                normal_index[v] = idx.normal_index;
            }

            for (size_t v = 0; v < fnum; v++) {
                float x = attrib.vertices[(vertex_index[v]) * 3 + 0];
                float y = attrib.vertices[(vertex_index[v]) * 3 + 1];
                float z = attrib.vertices[(vertex_index[v]) * 3 + 2];

                items.push_back(x);
                items.push_back(y);
                items.push_back(z);

                glm::vec3 position(x, y, z);

                minPosition = getMinPos(minPosition, position);
                maxPosition = getMaxPos(maxPosition, position);

                if (attrib.texcoords.empty()) {
                    items.push_back(0.0f);
                    items.push_back(0.0f);
                } else {
                    items.push_back(attrib.texcoords[texcoord_index[v] * 2 + 0]);
                    items.push_back(attrib.texcoords[texcoord_index[v] * 2 + 1]);
                }

                if (attrib.normals.empty()) {
                    items.push_back(1.0f);
                    items.push_back(0.2f);
                    items.push_back(0.3f);
                } else {
                    items.push_back(attrib.normals[normal_index[v] * 3 + 0]);
                    items.push_back(attrib.normals[normal_index[v] * 3 + 1]);
                    items.push_back(attrib.normals[normal_index[v] * 3 + 2]);
                }
            }

            index_offset += fnum;
        }

        data.push_back(items);
    }

    if (zero) {
        auto center = (minPosition + maxPosition) * 0.5f;

        minPosition -= center;
        maxPosition -= center;

        float maxValue = std::max(maxPosition.x, std::max(maxPosition.y, maxPosition.z));

        std::vector<std::vector<float>> tmp;

        for (auto mesh : data) {
            int count = mesh.size() >> 3;

            std::vector<float> unit;

            for (int i = 0; i < count; i++) {
                glm::vec3 position(mesh[i * 8 + 0], mesh[i * 8 + 1], mesh[i * 8 + 2]);
                position -= center;

                unit.push_back(position.x / maxValue);
                unit.push_back(position.y / maxValue);
                unit.push_back(position.z / maxValue);

                for (int j = 0; j < 5; j++)
                    unit.push_back(mesh[i * 8 + 3]);
            }

            tmp.push_back(unit);
        }

        minPosition /= maxValue;
        maxPosition /= maxValue;
        data = tmp;
    }

    return true;
}

glm::vec3 VK_OBJLoader::getMinPosition() const
{
    return minPosition;
}

glm::vec3 VK_OBJLoader::getMaxPosition() const
{
    return maxPosition;
}
