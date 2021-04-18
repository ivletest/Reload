//
// Created by ivan on 08.4.21.
//

#ifndef RELOAD_VULKAN_COMMON_H
#define RELOAD_VULKAN_COMMON_H

#include "ReloadLib/Containers/List.h"
#include "RenderCommon.h"

#include "../../lib/VulkanMemoryAllocator/include/vk_mem_alloc.h"

using ExtList = std::vector<const char *>;

struct GPU {
    int                                 score = -1;
    VkPhysicalDevice                    device = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties          props{};
    VkPhysicalDeviceMemoryProperties    memProps{};
    VkPhysicalDeviceFeatures            features{};
    VkSurfaceCapabilitiesKHR            surfaceCaps{};
    VkQueueFlags                        supportedQueues;

    std::vector<VkSurfaceFormatKHR>            surfaceFormats;
    std::vector<VkPresentModeKHR>              presentModes;
    std::vector<VkQueueFamilyProperties>       queueFamilyProps;
    std::vector<VkExtensionProperties>	        extensionProps;
};

struct RVkContext {
    GPU                     gpu{};
    VkDevice                device = VK_NULL_HANDLE;

    int                     graphicsFamilyIdx = -1;
    int                     presentFamilyIdx  = -1;
    int                     computeFamilyIdx  = -1;
    int                     transferFamilyIdx = -1;

    VkQueue	                graphicsQueue = VK_NULL_HANDLE;
    VkQueue	                presentQueue  = VK_NULL_HANDLE;
    VkQueue	                computeQueue  = VK_NULL_HANDLE;
    VkQueue	                transferQueue = VK_NULL_HANDLE;

    VkFormat				depthFormat = VK_FORMAT_UNDEFINED;
    VkRenderPass			renderPass = VK_NULL_HANDLE;
    VkPipelineCache			pipelineCache = VK_NULL_HANDLE;
    VkSampleCountFlagBits   sampleCount = VK_SAMPLE_COUNT_1_BIT;
    bool					superSampling = false;
};

struct RVkSwapchain {
    VkSwapchainKHR sc = VK_NULL_HANDLE;
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    VkExtent2D extent{};
    VkImage images[MAX_FRAMES_IN_FLIGHT];
    int imageCount = 0;
};

extern VkInstance       vkInstance;
extern RVkContext       vkContext;

#endif //RELOAD_VULKANCOMMON_H
