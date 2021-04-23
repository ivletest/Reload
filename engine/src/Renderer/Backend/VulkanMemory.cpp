//
// Created by ivan on 18.4.21.
//

#include "VulkanMemory.h"

#include "VulkanCommon.h"

VmaAllocator        vmaAllocator;
VmaAllocation       vmaAllocation;
VmaAllocationInfo	vmaAllocationInfo;

uint32_t FindMemoryTypeIndex(const uint32_t memTypeBits, const VkMemUsage usage) {
    VkPhysicalDeviceMemoryProperties & physicalMemoryProperties = vkContext.gpu.memProps;

    VkMemoryPropertyFlags required = 0;
    VkMemoryPropertyFlags preferred = 0;

    switch ( usage ) {
        case VK_MEMORY_USAGE_GPU_ONLY:
            preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        case VK_MEMORY_USAGE_CPU_ONLY:
            required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            break;
        case VK_MEMORY_USAGE_CPU_TO_GPU:
            required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        case VK_MEMORY_USAGE_GPU_TO_CPU:
            required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            preferred |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
            break;
        default:
            spdlog::critical( " Unknown memory usage." );
    }

    for (uint32_t i = 0; i < physicalMemoryProperties.memoryTypeCount; ++i) {
        if (((memTypeBits >> i) & 1) == 0 ) {
            continue;
        }

        const VkMemoryPropertyFlags properties = physicalMemoryProperties.memoryTypes[ i ].propertyFlags;
        if ( ( properties & required ) != required ) {
            continue;
        }

        if ( ( properties & preferred ) != preferred ) {
            continue;
        }

        return i;
    }

    for ( uint32_t i = 0; i < physicalMemoryProperties.memoryTypeCount; ++i ) {
        if (((memTypeBits >> i ) & 1) == 0) {
            continue;
        }

        const VkMemoryPropertyFlags properties = physicalMemoryProperties.memoryTypes[ i ].propertyFlags;
        if ( ( properties & required ) != required ) {
            continue;
        }

        return i;
    }

    return UINT32_MAX;
}