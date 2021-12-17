#include <cassert>
#include <malloc.h>
#include "VK_Allocator.h"

void* VK_Allocator::Allocation(size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    auto ret = _aligned_malloc(size, alignment);
    assert(ret == nullptr);
    return ret;
}

void* VKAPI_CALL VK_Allocator::Allocation(
    void*                   pUserData,
    size_t                  size,
    size_t                  alignment,
    VkSystemAllocationScope allocationScope)
{
    return static_cast<VK_Allocator*>(pUserData)->Allocation(size, alignment, allocationScope);
}

void* VK_Allocator::ReAllocation(
    void*                   pOriginal,
    size_t                  size,
    size_t                  alignment,
    VkSystemAllocationScope allocationScope)
{
    (void)allocationScope;
    return _aligned_realloc(pOriginal, size, alignment);
}

void* VKAPI_CALL VK_Allocator::ReAllocation(
    void*                   pUserData,
    void*                   pOriginal,
    size_t                  size,
    size_t                  alignment,
    VkSystemAllocationScope allocationScope)
{
    return static_cast<VK_Allocator*>(pUserData)->ReAllocation(
               pOriginal, size, alignment, allocationScope);
}

void VK_Allocator::Free(void *pMemory)
{
    _aligned_free(pMemory);
}

void VKAPI_CALL VK_Allocator::Free(void* pUserData, void* pMemory)
{
    return static_cast<VK_Allocator*>(pUserData)->Free(pMemory);
}
