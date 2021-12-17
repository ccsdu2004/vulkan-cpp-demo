#ifndef VK_ALLOCATOR_H
#define VK_ALLOCATOR_H
#include <vulkan/vulkan.h>

class VK_Allocator {
public:
    inline explicit operator VkAllocationCallbacks () const
    {
        VkAllocationCallbacks result;
        result.pUserData = (void*) this;
        result.pfnAllocation = &Allocation;
        result.pfnReallocation = &ReAllocation;
        result.pfnFree = &Free;
        result.pfnInternalAllocation = nullptr;
        result.pfnInternalFree = nullptr;
        return result;
    }
private:
    static void* VKAPI_CALL Allocation(
            void*                   pUserData,
            size_t                  size,
            size_t                  alignment,
            VkSystemAllocationScope allocationScope);

    static void* VKAPI_CALL ReAllocation(
            void*                   pUserData,
            void*                   pOriginal,
            size_t                  size,
            size_t                  alignment,
            VkSystemAllocationScope allocationScope);

    static void VKAPI_CALL Free(
            void*                   pUserData,
            void*                   pMemory);

    void* Allocation(
            size_t                  size,
            size_t                  alignment,
            VkSystemAllocationScope allocationScope);

    void* ReAllocation(
            void*                   pOriginal,
            size_t                  size,
            size_t                  alignment,
            VkSystemAllocationScope allocationScope);

    void Free(void* pMemory);
};

#endif // VK_ALLOCATOR_H
