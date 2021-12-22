#include <cassert>
#include <cstdlib>
#include <cstring>
#include <malloc.h>
#include "VK_Allocator.h"

void *allocationFunction(void *userData, size_t size, size_t alignment,
                         VkSystemAllocationScope allocationScope)
{
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

void freeFunction(void *userData, void *memory)
{
    free(memory);
}

void *reallocationFunction(void *userData, void *original, size_t size, size_t alignment,
                           VkSystemAllocationScope allocationScope)
{
    return realloc(original, size);
}

void internalAllocationNotification(void *userData, size_t size,
                                    VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
{
}

void internalFreeNotification(void *userData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
{
}

VkAllocationCallbacks *VK_Allocator::getAllocator()
{
    if (!allocationCallback)
        allocationCallback = new VkAllocationCallbacks();

    allocationCallback->pUserData = (void *)this;
    allocationCallback->pfnAllocation = (PFN_vkAllocationFunction)(&allocationFunction);
    allocationCallback->pfnReallocation = (PFN_vkReallocationFunction)(&reallocationFunction);
    allocationCallback->pfnFree = (PFN_vkFreeFunction)&freeFunction;
    allocationCallback->pfnInternalAllocation = (PFN_vkInternalAllocationNotification)
            &internalAllocationNotification;
    allocationCallback->pfnInternalFree = (PFN_vkInternalFreeNotification)&internalFreeNotification;
    return allocationCallback;
}

VK_Allocator::VK_Allocator()
{
}

VK_Allocator::~VK_Allocator()
{
    if (allocationCallback)
        delete allocationCallback;
}
