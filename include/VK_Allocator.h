#ifndef VK_ALLOCATOR_H
#define VK_ALLOCATOR_H
#include <vulkan/vulkan.h>

class VK_Allocator {
public:
    VK_Allocator();
    ~VK_Allocator();
public:
    VkAllocationCallbacks* getAllocator();
private:
    VkAllocationCallbacks* allocationCallback = nullptr;
};

#endif // VK_ALLOCATOR_H
