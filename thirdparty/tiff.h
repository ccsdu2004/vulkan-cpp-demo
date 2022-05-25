#pragma once

#include <stddef.h>
#include <vulkan/vulkan.h>

int writeTiff(char const * const filename, char const * const data,
              const VkExtent2D size, const size_t nchannels);

int readTiffRGBA(char const * const filename, VkExtent2D * const size,
                 VkSubresourceLayout * layout, uint8_t * const data) ;
