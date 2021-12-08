#ifndef VK_UTIL_H
#define VK_UTIL_H
#include <string>
#include <cstring>
#include <vector>
#include <vulkan/vulkan.h>

template<class T>
inline bool isSame(const std::vector<T>& a, const std::vector<T>& b)
{
    if(a.size() != b.size())
        return false;

    int size = sizeof(T) * a.size();
    return !memcmp((char*)a.data(), (char*)b.data(), size);
}

template<class C>
void cleanVulkanObjectContainer(C& container)
{
    while(true) {
        auto itr = container.begin();
        if (itr == container.end())
            break;
        else
            (*itr)->release();
    }
}
std::vector<char> readDataFromFile(const std::string &filename);

#endif // VK_UTIL_H
