#define VMA_IMPLEMENTATION

#if defined(__clang__)
#pragma clang diagnostic push "-Weverything"
#include "vk_mem_alloc.h"
#pragma clang diagnostic pop

#elif defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wtype-limits"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#include "vk_mem_alloc.h"
#pragma GCC diagnostic pop

#elif defined(_MSC_VER)
#pragma warning(push, 0)
#include "vk_mem_alloc.h"
#pragma warning(pop)
#endif
