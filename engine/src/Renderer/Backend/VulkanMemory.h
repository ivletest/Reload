//
// Created by ivan on 18.4.21.
//

#ifndef RELOAD_MEMORY_ALLOCATOR_H
#define RELOAD_MEMORY_ALLOCATOR_H

#include "precompiled.h"

#include "Common.h"

#if defined(_MSVC_LANG)
// Uncomment to test including `vulkan.h` on your own before including VMA.
    //#include <vulkan/vulkan.h>

    /*
    In every place where you want to use Vulkan Memory Allocator, define appropriate
    macros if you want to configure the library and then include its header to
    include all public interface declarations. Example:
    */

//#   define VMA_HEAVY_ASSERT(expr) assert(expr)
//#   define VMA_USE_STL_CONTAINERS 1
//#   define VMA_DEDICATED_ALLOCATION 0
//#   define VMA_DEBUG_MARGIN 16
//#   define VMA_DEBUG_DETECT_CORRUPTION 1
//#   define VMA_DEBUG_INITIALIZE_ALLOCATIONS 1
//#   define VMA_RECORDING_ENABLED 1
//#   define VMA_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY 256
//#   define VMA_USE_STL_SHARED_MUTEX 0
//#   define VMA_DEBUG_GLOBAL_MUTEX 1
//#   define VMA_MEMORY_BUDGET 0
#   define VMA_STATIC_VULKAN_FUNCTIONS 0
#   define VMA_DYNAMIC_VULKAN_FUNCTIONS 1

#   define VMA_VULKAN_VERSION 1002000 // Vulkan 1.2
//#   define VMA_VULKAN_VERSION 1001000 // Vulkan 1.1
//#   define VMA_VULKAN_VERSION 1000000 // Vulkan 1.0

    /*
    #define VMA_DEBUG_LOG(format, ...) do { \
            printf(format, __VA_ARGS__); \
            printf("\n"); \
        } while(false)
    */

#   pragma warning(push, 4)
#   pragma warning(disable: 4127) // conditional expression is constant
#   pragma warning(disable: 4100) // unreferenced formal parameter
#   pragma warning(disable: 4189) // local variable is initialized but not referenced
#   pragma warning(disable: 4324) // structure was padded due to alignment specifier

#endif  // #ifdef _MSVC_LANG

#if defined(__clang__)
#   pragma clang diagnostic push
#       pragma clang diagnostic ignored "-Wtautological-compare" // comparison of unsigned expression < 0 is always false
#       pragma clang diagnostic ignored "-Wunused-private-field"
#       pragma clang diagnostic ignored "-Wunused-parameter"
#       pragma clang diagnostic ignored "-Wmissing-field-initializers"
#       pragma clang diagnostic ignored "-Wnullability-completeness"
#endif

#if defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic push
#       pragma GCC diagnostic ignored "-Wunused-parameter"
#       pragma GCC diagnostic ignored "-Wunused-variable"
#       pragma GCC diagnostic ignored "-Wconversion"
#       pragma GCC diagnostic ignored "-Wtype-limits"
#       pragma GCC diagnostic ignored "-Wmissing-field-initializers"

#endif

#include "lib/vma/vk_mem_alloc.h"

#if defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#if defined(_MSVC_LANG)
#   pragma warning(pop)
#endif

enum VkMemUsage {
    VK_MEMORY_USAGE_UNKNOWN,
    VK_MEMORY_USAGE_GPU_ONLY,
    VK_MEMORY_USAGE_CPU_ONLY,
    VK_MEMORY_USAGE_CPU_TO_GPU,
    VK_MEMORY_USAGE_GPU_TO_CPU,
    VK_MEMORY_USAGES,
};

uint32_t FindMemoryTypeIndex(uint32_t memTypeBits, VkMemUsage usage);

extern VmaAllocator         vmaAllocator;
extern VmaAllocation        vmaAllocation;
extern VmaAllocationInfo	vmaAllocationInfo;

#endif //RELOAD_MEMORY_ALLOCATOR_H
