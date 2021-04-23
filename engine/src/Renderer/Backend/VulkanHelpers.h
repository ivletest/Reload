//
// Created by ivan on 11.4.21.
//

#ifndef RELOAD_VULKAN_HELPERS_H
#define RELOAD_VULKAN_HELPERS_H

#include "precompiled.h"

#include "VulkanCommon.h"

/*
================================================================================
VkErrorToString

DESCRIPTION:
Converts VkResult enum value to string and returns it.
================================================================================
*/
[[maybe_unused]] inline const char * VkErrorToString(VkResult result) {
    switch (result) {
        case VK_SUCCESS:
            return "Success";
        case VK_NOT_READY:
            return "A fence or query has not yet completed";
        case VK_TIMEOUT:
            return "A wait operation has not completed in time";
        case VK_EVENT_SET:
            return "An event is signaled";
        case VK_EVENT_RESET:
            return "An event is unsignaled";
        case VK_INCOMPLETE:
            return "A return array was too small for the result";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "A host memory allocation has failed";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "A device memory allocation has failed";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "Initialization of an object could not be completed";
        case VK_ERROR_DEVICE_LOST:
            return "The device or physical device has been lost";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "Mapping of a memory object has failed";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "A requested layer is not present or could not be loaded";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "A requested extension is not supported";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "A requested feature is not supported";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "The Vulkan version is not supported by the driver";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "Too many objects of the type have already been created";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "A requested format is not supported on this device";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "A surface is no longer available";
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            return "A allocation failed. No more space in the descriptor pool";
        case VK_SUBOPTIMAL_KHR:
            return "A swapchain no longer matches the surface properties, but can still be used";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "A surface has changed, and is no longer compatible with the swapchain";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "The display used by a swapchain does not use the same presentable image layout";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "The requested window is already connected to a VkSurfaceKHR, or to some other non-Vulkan API";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "A validation layer found an error";
        default:
            return "Unknown Vulkan error";
    }
}

#define VK_CHECK(x) {                                                          \
	VkResult ret = x;                                                          \
	if (ret != VK_SUCCESS) {                                                   \
        spdlog::critical("VK: {0} - {1}", VkErrorToString(ret), #x); \
        exit(1);                                                               \
    }                                                                          \
}

#define VK_VALIDATE(x, msg)                                                    \
	if (!(x)) {                                                                \
        spdlog::critical("VK: {0} - {1}", msg, #x);                   \
        exit(1);                                                               \
    }

[[maybe_unused]] inline VkImageView RVkCreateImageView(VkImage img, VkFormat fmt, VkImageAspectFlags flags) {
    VkImageViewCreateInfo viewInfo = {};
    VkImageView imageView;

    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = img;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = fmt;
    viewInfo.subresourceRange.aspectMask = flags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(vkContext.device, &viewInfo, nullptr, &imageView))

    return imageView;
}

#endif //RELOAD_VULKAN_HELPERS_H
