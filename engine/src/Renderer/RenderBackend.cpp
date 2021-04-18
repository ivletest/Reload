#include "RenderBackend.h"

#include "ConfigManager.h"
#include "Windowing/Window.h"
#include "VulkanHelpers.h"
#include "StagingManager.h"
#include "Image.h"
#include "ImageManager.h"
#include "RenderState.h"
#include "RenderLog.h"

extern VmaAllocation        vmaAllocation;
extern VmaAllocationInfo	vmaAllocationInfo;

VkInstance      vkInstance;
RVkContext      vkContext;

const char * VK_LAYER_KHRONOS_VALIDATION_NAME = "VK_LAYER_KHRONOS_validation";

static VkDebugReportCallbackEXT s_debugReportCallback = VK_NULL_HANDLE;

static ExtList g_debugExtensions({VK_EXT_DEBUG_REPORT_EXTENSION_NAME });
static ExtList g_deviceExtensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME });
static ExtList g_validationLayers({VK_LAYER_KHRONOS_VALIDATION_NAME });

// Internal Helpers

/*
================================================================================
CheckValidationLayers

DESCRIPTION:
Checks whether the graphic device supports the validation layers selected.
If the validation fails it exits the application.
================================================================================
*/
static void CheckValidationLayers() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> instanceLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, instanceLayers.data());

    bool layerFound = false;
    for (auto enabledValidationLayer : g_validationLayers) {
        for (size_t j = 0; j < layerCount; j++) {
            VkLayerProperties layerProps = instanceLayers[j];
            if (!strcmp(enabledValidationLayer, layerProps.layerName)) {
                layerFound = true;
                break;
            }
        }
        if(!layerFound) {
            spdlog::critical(
                    "Validation layer {} not supported by graphics device.",
                    enabledValidationLayer);
            exit(1);
        }
    }
}

/*
================================================================================
ChooseSurfaceFormat
DESCRIPTION:
Chooses the appropriate m_surface format.
================================================================================
*/
VkSurfaceFormatKHR ChooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> & formats) {
    VkSurfaceFormatKHR result;
    size_t formatsCount = formats.size();

    if (formatsCount == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
        result.format = VK_FORMAT_B8G8R8A8_UNORM;
        result.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        return result;
    }

    for (size_t i = 0; i < formatsCount; i++) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB // VK_FORMAT_B8G8R8A8_UNORM
            && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return formats[i];
        }
    }

    return formats[0];
}

/*
================================================================================
DebugCallback

DESCRIPTION:
Logs the debug info message in the specified format.
================================================================================
*/

#if defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#elif defined(_MSC_VER)
#pragma warning( push )
    #pragma warning( disable : 4100) // Unused parameter
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char * layerPrefix,
        const char * msg,
        [[maybe_unused]] void * userData) {

    SDL_LogDebug(
            LOG_RENDER,
            "VK_DEBUG::%s: %s flags=%d, objType=%d, obj=%lu, location=%ld, code=%d\n",
            layerPrefix, msg, flags, objType, obj, location, code);

    return VK_FALSE;
}

#if defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning( pop )
#endif

void CreateDebugReportCallback(VkInstance instance) {
    VkDebugReportCallbackCreateInfoEXT callbackInfo = {};
    callbackInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    callbackInfo.flags = VK_DEBUG_REPORT_DEBUG_BIT_EXT
                         | VK_DEBUG_REPORT_WARNING_BIT_EXT
                         | VK_DEBUG_REPORT_ERROR_BIT_EXT
                         | VK_DEBUG_REPORT_INFORMATION_BIT_EXT
                         | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    callbackInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT) DebugCallback;

    PFN_vkCreateDebugReportCallbackEXT func;
    func = (PFN_vkCreateDebugReportCallbackEXT)SDL_Vulkan_GetVkGetInstanceProcAddr();

    VK_VALIDATE(func != nullptr, "Could not find vkCreateDebugReportCallbackEXT")
    VK_CHECK(func(instance, &callbackInfo, nullptr, &s_debugReportCallback))
}


/*
================================================================================
DestroyDebugReportCallback
DESCRIPTION:
Destroys the debug report callback.
================================================================================
*/
static void DestroyDebugReportCallback(VkInstance instance) {
    PFN_vkDestroyDebugReportCallbackEXT func;
    func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");

    VK_VALIDATE(func != nullptr, "Could not find vkDestroyDebugReportCallbackEXT")
    func(instance, s_debugReportCallback, nullptr);
}

/*
================================================================================
CreatePipelineCache
DESCRIPTION:
Creates a pipline cache.
================================================================================
*/
static void CreatePipelineCache() {
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    VK_CHECK(vkCreatePipelineCache(
            vkContext.device,
            &pipelineCacheCreateInfo,
            nullptr,
            &vkContext.pipelineCache))
}

/*
================================================================================
ChooseSupportedFormat

DESCRIPTION:
Chooses the supported depth format.
================================================================================
*/
static VkFormat ChooseSupportedFormat(
        const VkFormat * formats,
        int numFormats,
        VkImageTiling tiling,
        VkFormatFeatureFlags features) {

    for ( int i = 0; i < numFormats; ++i ) {
        VkFormat format = formats[i];

        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(vkContext.gpu.device, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    Log_GpuError("Failed to find a supported format.");

    return VK_FORMAT_UNDEFINED;
}

/*
================================================================================
RenderBackend::RenderBackend

DESCRIPTION:
The render backend's constructor.
================================================================================
*/
RenderBackend::RenderBackend() {
    ClearContext();
    Clear();
}

/*
================================================================================
RenderBackend::~RenderBackend

DESCRIPTION:
The render backend's destructor.
================================================================================
*/
RenderBackend::~RenderBackend() {
    Clear();
}

/*
================================================================================
RenderBackend::ClearContext

DESCRIPTION:
Clears the vulkan context and resets all the values.
================================================================================
*/
void RenderBackend::ClearContext() {
    vkContext.gpu = GPU();
    vkContext.device = VK_NULL_HANDLE;
    vkContext.graphicsQueue = VK_NULL_HANDLE;
    vkContext.presentQueue = VK_NULL_HANDLE;
    vkContext.depthFormat = VK_FORMAT_UNDEFINED;
    vkContext.renderPass = VK_NULL_HANDLE;
    vkContext.pipelineCache = VK_NULL_HANDLE;
    vkContext.sampleCount = VK_SAMPLE_COUNT_1_BIT;
    vkContext.superSampling = false;
}

/*
================================================================================
RenderBackend::Clear

DESCRIPTION:
Clears and resets all the values.
================================================================================
*/
void RenderBackend::Clear() {
    m_counter = 0;
    m_currentFrame = 0;

    vkInstance = VK_NULL_HANDLE;

    s_debugReportCallback = VK_NULL_HANDLE;

    m_fullscreen = 0;
    m_swapchain = VK_NULL_HANDLE;
    m_swapchainFormat = VK_FORMAT_UNDEFINED;
    m_currentSwapIdx = 0;

    m_currentImage = VK_NULL_HANDLE;
    m_currentImageView = VK_NULL_HANDLE;
    m_commandPool = VK_NULL_HANDLE;

    std::fill(m_swapchainImages.begin(), m_swapchainImages.end(), nullptr);
    std::fill(m_swapchainViews.begin(), m_swapchainViews.end(), nullptr);
    std::fill(m_frameBuffers.begin(), m_frameBuffers.end(), nullptr);
    std::fill(m_commandBuffers.begin(), m_commandBuffers.end(), nullptr);
    std::fill(m_inFlightFences.begin(), m_inFlightFences.end(), nullptr);
    std::fill(m_commandBufferRecorded.begin(), m_commandBufferRecorded.end(), false);
    std::fill(m_imgAvailableSemaphores.begin(), m_imgAvailableSemaphores.end(), nullptr);
    std::fill(m_renderCompleteSemaphores.begin(), m_renderCompleteSemaphores.end(), nullptr);
    std::fill(m_queryIndex.begin(), m_queryIndex.end(), 0);

    for (auto & queryResult : m_queryResults) {
        std::fill(queryResult.begin(), queryResult.end(), 0);
    }

    std::fill(m_queryPools.begin(), m_queryPools.end(), nullptr);
}

/*
================================================================================
RenderBackend::Init

DESCRIPTION:
Initializes the render backend.
================================================================================
*/
void RenderBackend::Init() {
    windowManager.EnumerateDisplays();
    windowManager.CreateWindow();

    VK_CHECK(volkInitialize())

    CreateInstance();
    CreateSurface();
    SelectBestGpu();
    CreateLogicalDeviceAndQueues();
    CreateQueryPool();
    CreateCommandPool();
    CreateCommandBuffer();

    VmaVulkanFunctions funcs {};
    funcs.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
    funcs.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
    funcs.vkAllocateMemory = vkAllocateMemory;
    funcs.vkFreeMemory = vkFreeMemory;
    funcs.vkMapMemory = vkMapMemory;
    funcs.vkUnmapMemory = vkUnmapMemory;
    funcs.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
    funcs.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
    funcs.vkBindBufferMemory = vkBindBufferMemory;
    funcs.vkBindImageMemory = vkBindImageMemory;
    funcs.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
    funcs.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
    funcs.vkCreateBuffer = vkCreateBuffer;
    funcs.vkDestroyBuffer = vkDestroyBuffer;
    funcs.vkCreateImage = vkCreateImage;
    funcs.vkDestroyImage = vkDestroyImage;
    funcs.vkCmdCopyBuffer = vkCmdCopyBuffer;
    funcs.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
    funcs.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;

    VmaAllocatorCreateInfo vmaInfo = {};
    vmaInfo.physicalDevice = vkContext.gpu.device;
    vmaInfo.device = vkContext.device;
    vmaInfo.instance = vkInstance;
    vmaInfo.pVulkanFunctions = &funcs;
//    createInfo.preferredSmallHeapBlockSize = vkConfig.uploadBufferSizeMB * 1024 * 1024;
//    createInfo.preferredLargeHeapBlockSize = vkConfig.deviceLocalMemoryMB * 1024 * 1024;
    vmaCreateAllocator(&vmaInfo, &vmaAllocator);

    stagingManager.Init();
    CreateSwapchain();
    CreateRenderTargets();
    CreateRenderPass();
    CreatePipelineCache();
    CreateFrameBuffers();
    CreateSyncObjects();
}

/*
================================================================================
RenderBackend::Shutdown

DESCRIPTION:
Shuts down the render backend.
================================================================================
*/
void RenderBackend::Shutdown() {
    DestroySyncObjects();
    DestroyFrameBuffers();

    vkDestroyPipelineCache(vkContext.device, vkContext.pipelineCache, nullptr);
    vkDestroyRenderPass(vkContext.device, vkContext.renderPass, nullptr);

    DestroyRenderTargets();
    DestroySwapchain();

    stagingManager.Shutdown();

    vmaDestroyAllocator(vmaAllocator);
    vkFreeCommandBuffers(vkContext.device, m_commandPool, MAX_FRAMES_IN_FLIGHT, m_commandBuffers.data());
    vkDestroyCommandPool(vkContext.device, m_commandPool, nullptr);
    DestroyQueryPool();

    if (vkConfig.enableDebugLayer) {
        DestroyDebugReportCallback(vkInstance);
    }

    vkDestroyDevice(vkContext.device, nullptr);
    vkDestroySurfaceKHR(vkInstance, m_surface, nullptr);
    vkDestroyInstance(vkInstance, nullptr);

    ClearContext();
    Clear();

    windowManager.DestroyWindow();
}

/*
================================================================================
RenderBackend::Restart

DESCRIPTION:
Restarts the render backend.
================================================================================
*/
void RenderBackend::Restart() {

}

/*
================================================================================
RenderBackend::CreateInstance

DESCRIPTION:
Configures and creates the Vulkan instance.
================================================================================
*/
void RenderBackend::CreateInstance() {
    uint32_t appVer = VK_MAKE_VERSION(vkConfig.programVersion.major,
                                      vkConfig.programVersion.minor,
                                      vkConfig.programVersion.patch);

    uint32_t engineVer = VK_MAKE_VERSION(vkConfig.engineVersion.major,
                                         vkConfig.engineVersion.minor,
                                         vkConfig.engineVersion.patch);

    uint32_t vulkanApiVersion;
    VK_CHECK(vkEnumerateInstanceVersion(&vulkanApiVersion))

    VkApplicationInfo appInfo  = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = VK_NULL_HANDLE;
    appInfo.pApplicationName = vkConfig.programName;
    appInfo.applicationVersion = appVer;
    appInfo.pEngineName = vkConfig.engineName;
    appInfo.engineVersion = engineVer;
    appInfo.apiVersion = vulkanApiVersion;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    ;
    m_deviceExtensions.clear();
    m_validationLayers.clear();

    uint32_t extCount;
    SDL_Vulkan_GetInstanceExtensions(window.handle, &extCount, nullptr);
    std::vector<const char *> instanceExts(extCount);
    SDL_Vulkan_GetInstanceExtensions(window.handle, &extCount, instanceExts.data());

    for (auto & deviceExtension : g_deviceExtensions) {
        m_deviceExtensions.push_back(deviceExtension);
    }

    if (vkConfig.enableValidationLayers) {
        for (auto & debugExtension : g_debugExtensions) {
            instanceExts.push_back(debugExtension);
        }

        for (auto & validationLayer : g_validationLayers) {
            m_validationLayers.push_back(validationLayer);
        }

        CheckValidationLayers();
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExts.size());
    createInfo.ppEnabledExtensionNames = instanceExts.data();
    createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
    createInfo.ppEnabledLayerNames = m_validationLayers.data();

    VK_CHECK(vkCreateInstance(&createInfo, nullptr, &vkInstance))
    volkLoadInstance(vkInstance);

    if (vkConfig.enableDebugLayer) {
        CreateDebugReportCallback(vkInstance);
    }
}

/*
================================================================================
RenderBackend::CreateSurface

DESCRIPTION:
Creates the presentation surface.
================================================================================
*/
void RenderBackend::CreateSurface() {
    if (!SDL_Vulkan_CreateSurface(window.handle, vkInstance, &m_surface)) {
        SDL_LogCritical(LOG_VIDEO, "Cannot create Vulkan window surface");
        exit(1);
    }
}

/*
================================================================================
RenderBackend::CheckExtSupport

DESCRIPTION:
Checks whether the physical graphics device supports all the extensions required
to run the program.
================================================================================
*/
static bool CheckExtSupport(GPU & info, std::vector<const char *> & requiredExt) {
    size_t extensionPropsCount = info.extensionProps.size();
    size_t required = requiredExt.size();
    size_t available = 0;

    for (size_t i = 0; i < required; ++i) {
        for (size_t j = 0; j < extensionPropsCount; ++j ) {
            if (strcmp(requiredExt[i], info.extensionProps[j].extensionName) == 0) {
                available++;
                break;
            }
        }
    }

    return available == required;
}

/*
================================================================================
RenderBackend::GetPhysicalDeviceInfo

DESCRIPTION:
Gets the physical graphics device properties required to rate and check whether
it is capable of running the program.

RETURNS:
GPU struct containing the required information.
================================================================================
*/

GPU RenderBackend::GetDeviceInfo(VkPhysicalDevice device) {
    GPU gpu;
    gpu.device = device;

    // Get the queues from the device
    // -------------------------------------------------------------------------
    uint32_t numQueues = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu.device, &numQueues, nullptr);

    VK_VALIDATE(numQueues > 0, "No queue families supported by device")

    gpu.queueFamilyProps.resize(numQueues);
    vkGetPhysicalDeviceQueueFamilyProperties(
            gpu.device,
            &numQueues,
            gpu.queueFamilyProps.data());


    // Get the supported extensions by the device
    // -------------------------------------------------------------------------
    uint32_t numExtensions;
    VK_CHECK(vkEnumerateDeviceExtensionProperties(
            gpu.device,
            nullptr,
            &numExtensions, nullptr))

    VK_VALIDATE(numExtensions > 0, "No extensions supported by device" )

    gpu.extensionProps.resize(numExtensions);
    VK_CHECK(vkEnumerateDeviceExtensionProperties(
            gpu.device,
            nullptr,
            &numExtensions,
            gpu.extensionProps.data()))

    // Get the m_surface capabilities
    // -------------------------------------------------------------------------
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            gpu.device,
            m_surface,
            &gpu.surfaceCaps))

    // Get the supported m_surface formats (image format and color space)
    // -------------------------------------------------------------------------
    uint32_t numFormats;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
            gpu.device,
            m_surface,
            &numFormats,
            nullptr))

    VK_VALIDATE(numFormats > 0, "No m_surface formats supported by device")

    gpu.surfaceFormats.resize(numFormats);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
            gpu.device,
            m_surface,
            &numFormats,
            gpu.surfaceFormats.data()))


    // Get the supported presentation modes
    // -------------------------------------------------------------------------
    uint32_t numPresentModes;
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            gpu.device,
            m_surface,
            &numPresentModes,
            nullptr))

    VK_VALIDATE(numPresentModes > 0, "No present modes supported by device")

    gpu.presentModes.resize(numPresentModes);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            gpu.device,
            m_surface,
            &numPresentModes,
            gpu.presentModes.data()))

    // Get the memory types supported by the physical device used to allocate
    // memory for buffers, images, etc. Also the physical device properties
    // containing all possible hardware limits, and additional features
    // that are supported.
    // -------------------------------------------------------------------------
    vkGetPhysicalDeviceMemoryProperties(gpu.device, &gpu.memProps);
    vkGetPhysicalDeviceProperties(gpu.device, &gpu.props);
    vkGetPhysicalDeviceFeatures(gpu.device, &gpu.features);

    if (gpu.props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        gpu.score += 1000;
    }

    gpu.score += gpu.props.limits.maxImageDimension2D;

    return gpu;
}

/*
================================================================================
RenderBackend::SelectBestGpu

DESCRIPTION:
Selects the best graphics device available on the system.
================================================================================
*/
void RenderBackend::SelectBestGpu() {
    uint32_t deviceCount = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr))
    VK_VALIDATE(deviceCount > 0, "Can't find Vulkan enabled device.")

    std::vector<VkPhysicalDevice> devices(deviceCount);
    VK_CHECK(vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data()))

    int bestScore = -1;
    GPU bestGpu;

    for (VkPhysicalDevice &device : devices) {
        GPU gpu = GetDeviceInfo(device);

        if (gpu.score >= bestScore) {
            bestScore = gpu.score;
            bestGpu = gpu;
        }
    }

#ifdef RLD_DEBUG
    Log_PhysicalDeviceInfo(bestGpu);
#endif

    if (!CheckExtSupport(bestGpu, m_deviceExtensions)) {
        Log_GpuCritical("GPU doesn't support required extensions");
        exit(1);
    }

    if (bestGpu.surfaceFormats.empty()) {
        Log_GpuCritical("GPU doesn't support surface formats.");
        exit(1);
    }

    if (bestGpu.presentModes.empty()) {
        Log_GpuCritical("GPU doesn't support present modes.");
        exit(1);
    }

    // Find graphics queue family
    size_t numQueueFamilyProps = bestGpu.queueFamilyProps.size();

    for (size_t i = 0; i < numQueueFamilyProps; ++i) {

        if (bestGpu.queueFamilyProps[i].queueCount == 0) {
            continue;
        }

        // Check for graphics support
        if (bestGpu.queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            vkContext.graphicsFamilyIdx = static_cast<int>(i);
            vkContext.gpu.supportedQueues |= VK_QUEUE_GRAPHICS_BIT;
        }

        // Check for present support. Usually ends up the same as the graphics
        // family that's why there is no continue in previous condition.
        VkBool32 supportsPresent = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(
                bestGpu.device,
                static_cast<uint32_t>(i),
                m_surface,
                &supportsPresent);

        if (supportsPresent) {
            vkContext.presentFamilyIdx = static_cast<int>(i);
            continue;
        }

        // Check for compute support.
        if (bestGpu.queueFamilyProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            vkContext.computeFamilyIdx = static_cast<int>(i);
            vkContext.gpu.supportedQueues |= VK_QUEUE_COMPUTE_BIT;
            continue;
        }

        // Check for transfer support.
        if (bestGpu.queueFamilyProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            vkContext.transferFamilyIdx = static_cast<int>(i);
            vkContext.gpu.supportedQueues |= VK_QUEUE_TRANSFER_BIT;
            continue;
        }
    }

    if (vkContext.graphicsFamilyIdx < 0 || vkContext.presentFamilyIdx < 0) {
        Log_GpuCritical("GPU doesn't meet the minimum requirements");
        exit(1);
    }

    if (vkContext.computeFamilyIdx < 0) {
        Log_GpuWarn("GPU doesn't support compute queue family");
    }

    if (vkContext.transferFamilyIdx < 0) {
        Log_GpuWarn("GPU doesn't support transfer queue family");
    }

    vkContext.gpu = bestGpu;
}

/*
================================================================================
RenderBackend::CreateLogicalDeviceAndQueues

DESCRIPTION:
Selects the suitable graphics device.
================================================================================
*/
void RenderBackend::CreateLogicalDeviceAndQueues() {
    std::set<uint32_t> qFamilyIndices;
    qFamilyIndices.insert(static_cast<uint32_t>(vkContext.graphicsFamilyIdx));
    qFamilyIndices.insert(static_cast<uint32_t>(vkContext.presentFamilyIdx));

    if (vkContext.gpu.supportedQueues & VK_QUEUE_COMPUTE_BIT) {
        qFamilyIndices.insert(static_cast<uint32_t>(vkContext.computeFamilyIdx));
    }
    if (vkContext.gpu.supportedQueues & VK_QUEUE_TRANSFER_BIT) {
        qFamilyIndices.insert(static_cast<uint32_t>(vkContext.transferFamilyIdx));
    }

    std::vector<VkDeviceQueueCreateInfo> queueInfos;
    const float queuePriority = 1.0f;

    for (auto & familyIdx : qFamilyIndices) {
        VkDeviceQueueCreateInfo qInfo = {};
        qInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qInfo.queueFamilyIndex = familyIdx;
        qInfo.queueCount  = 1;
        qInfo.pQueuePriorities = &queuePriority;

        queueInfos.push_back(qInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.textureCompressionBC = VK_TRUE;
    deviceFeatures.imageCubeArray = VK_TRUE;
    deviceFeatures.depthClamp = VK_TRUE;
    deviceFeatures.depthBiasClamp = VK_TRUE;
    deviceFeatures.depthBounds = vkContext.gpu.features.depthBounds;
    deviceFeatures.fillModeNonSolid  = VK_TRUE;
    deviceFeatures.sampleRateShading = vkContext.gpu.features.sampleRateShading;
    deviceFeatures.samplerAnisotropy = vkContext.gpu.features.samplerAnisotropy;
    deviceFeatures.geometryShader = vkContext.gpu.features.geometryShader;
    deviceFeatures.tessellationShader = vkContext.gpu.features.tessellationShader;
    deviceFeatures.multiViewport = vkContext.gpu.features.multiViewport;
    deviceFeatures.vertexPipelineStoresAndAtomics = vkContext.gpu.features.vertexPipelineStoresAndAtomics;
    deviceFeatures.fragmentStoresAndAtomics = vkContext.gpu.features.fragmentStoresAndAtomics;
    deviceFeatures.shaderStorageImageExtendedFormats = vkContext.gpu.features.shaderStorageImageExtendedFormats;
    deviceFeatures.shaderStorageImageWriteWithoutFormat = vkContext.gpu.features.shaderStorageImageWriteWithoutFormat;

    if (vkContext.gpu.features.textureCompressionBC) {
        deviceFeatures.textureCompressionBC = VK_TRUE;
    }
    else if (vkContext.gpu.features.textureCompressionASTC_LDR) {
        deviceFeatures.textureCompressionASTC_LDR = VK_TRUE;
    }
    else if (vkContext.gpu.features.textureCompressionETC2) {
        deviceFeatures.textureCompressionETC2 = VK_TRUE;
    }

    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
    deviceInfo.pQueueCreateInfos = queueInfos.data();
    deviceInfo.pEnabledFeatures = &deviceFeatures;
    deviceInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
    deviceInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

    if (vkConfig.enableValidationLayers) {
        deviceInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
        deviceInfo.ppEnabledLayerNames = m_validationLayers.data();
    } else {
        deviceInfo.enabledLayerCount = 0;
    }

    VK_CHECK(vkCreateDevice(vkContext.gpu.device, &deviceInfo, nullptr, &vkContext.device))
    volkLoadDevice(vkContext.device);
    
    vkGetDeviceQueue(
            vkContext.device,
            static_cast<uint32_t>(vkContext.graphicsFamilyIdx),
            0,
            &vkContext.graphicsQueue);
    vkGetDeviceQueue(
            vkContext.device,
            static_cast<uint32_t>(vkContext.presentFamilyIdx),
            0,
            &vkContext.presentQueue);

    if(vkContext.gpu.supportedQueues & VK_QUEUE_COMPUTE_BIT) {
        vkGetDeviceQueue(
                vkContext.device,
                static_cast<uint32_t>(vkContext.computeFamilyIdx),
                0,
                &vkContext.computeQueue);
    }
    if(vkContext.gpu.supportedQueues & VK_QUEUE_TRANSFER_BIT) {
        vkGetDeviceQueue(
                vkContext.device,
                static_cast<uint32_t>(vkContext.transferFamilyIdx),
                0,
                &vkContext.transferQueue);
    }
}

/*
================================================================================
ChoosePresentMode

DESCRIPTION:
Chooses the appropriate presentation mode.
================================================================================
*/
static VkPresentModeKHR ChoosePresentMode(std::vector<VkPresentModeKHR> & modes) {
    size_t modesCount = modes.size();
    for (size_t i = 0; i < modesCount; i++) {
        if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return modes[i];
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

/*
================================================================================
ChooseSurfaceExtent

DESCRIPTION:
Chooses the m_surface extent (size) which is typically the size of the window.
================================================================================
*/
static VkExtent2D ChooseSurfaceExtent(VkSurfaceCapabilitiesKHR & caps) {

    if (caps.currentExtent.width != UINT32_MAX) {
        return caps.currentExtent;
    } else {
        VkExtent2D extent;
        extent.width = std::max(
                caps.minImageExtent.width,
                std::min(caps.maxImageExtent.width, static_cast<uint32_t>(window.width)));
        extent.height = std::max(
                caps.minImageExtent.height,
                std::min(caps.maxImageExtent.height, static_cast<uint32_t>(window.height)));

        return extent;
    }
}

/*
================================================================================
RenderBackend::CreateSwapchain

DESCRIPTION:
Creates the swap chain.
================================================================================
*/
void RenderBackend::CreateSwapchain() {
    GPU & gpu = vkContext.gpu;
    VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(gpu.surfaceFormats);
    VkPresentModeKHR presentMode = ChoosePresentMode(gpu.presentModes);
    VkExtent2D extent = ChooseSurfaceExtent(gpu.surfaceCaps);

    VkSwapchainCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = m_surface;
    info.minImageCount = MAX_FRAMES_IN_FLIGHT;
    info.imageFormat = surfaceFormat.format;
    info.imageColorSpace = surfaceFormat.colorSpace;
    info.imageExtent = extent;
    info.imageArrayLayers = 1; // Unless developing a stereoscopic 3D application, this should always be 1.

    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                    | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    if (vkContext.graphicsFamilyIdx != vkContext.presentFamilyIdx) {
        uint32_t indices[] = {
                static_cast<uint32_t>(vkContext.graphicsFamilyIdx),
                static_cast<uint32_t>(vkContext.presentFamilyIdx)
        };

        info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        info.queueFamilyIndexCount = 2;
        info.pQueueFamilyIndices = indices;
    } else {
        info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.presentMode = presentMode;
    info.clipped = VK_TRUE;
    info.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(vkContext.device, &info, nullptr, &m_swapchain))

    m_swapchainFormat = surfaceFormat.format;
    m_presentMode = presentMode;
    m_swapchainExtent = extent;
    m_fullscreen = windowConfig.isFullScreen;

    uint32_t numImages = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(vkContext.device, m_swapchain, &numImages, nullptr))
    VK_VALIDATE(numImages > 0, "vkGetSwapchainImagesKHR returned a zero image count.")

    VK_CHECK(vkGetSwapchainImagesKHR(vkContext.device, m_swapchain, &numImages, m_swapchainImages.data()))

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i ) {
        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = m_swapchainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = m_swapchainFormat;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        imageViewCreateInfo.flags = 0;

        VK_CHECK(vkCreateImageView(vkContext.device, &imageViewCreateInfo, nullptr, &m_swapchainViews[i]))
    }
}

/*
================================================================================
RenderBackend::DestroySwapchain

DESCRIPTION:
Destroys the swap chain.
================================================================================
*/
void RenderBackend::DestroySwapchain() {
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyImageView(vkContext.device, m_swapchainViews[i], nullptr);
    }

    vkDestroySwapchainKHR(vkContext.device, m_swapchain, nullptr);
}

/*
================================================================================
RenderBackend::CreateFrameBuffers

DESCRIPTION:
Creates frame buffers.
================================================================================
*/
void RenderBackend::CreateFrameBuffers() {

    VkImageView attachments[3] = {};

    Image * depthImg = globalImages->GetImage("_viewDepth");
    if (depthImg == nullptr ) {
        spdlog::critical( "CreateFrameBuffers: No _viewDepth image." );
    } else {
        attachments[1] = depthImg->GetView();
    }

    const bool resolve = vkContext.sampleCount > VK_SAMPLE_COUNT_1_BIT;
    if (resolve) {
        attachments[2] = m_currentImageView;
    }

    VkFramebufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.renderPass = vkContext.renderPass;
    frameBufferCreateInfo.attachmentCount = resolve ? 3 : 2;
    frameBufferCreateInfo.pAttachments = attachments;
    frameBufferCreateInfo.width = m_swapchainExtent.width;
    frameBufferCreateInfo.height = m_swapchainExtent.height;
    frameBufferCreateInfo.layers = 1;

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i ) {
        attachments[0] = m_swapchainViews[i];
        VK_CHECK(vkCreateFramebuffer(
                vkContext.device,
                &frameBufferCreateInfo,
                nullptr,
                &m_frameBuffers[i]))
    }
}

/*
================================================================================
RenderBackend::DestroyFrameBuffers

DESCRIPTION:
Destroys frame buffers.
================================================================================
*/
void RenderBackend::DestroyFrameBuffers() {
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyFramebuffer(vkContext.device, m_frameBuffers[i], nullptr);
    }
}


/*
================================================================================
RenderBackend::CreateSyncObjects

DESCRIPTION:
Creates the required semaphores and fences.
================================================================================
*/
void RenderBackend::CreateSyncObjects() {
    // Create semaphores:
//    VkSemaphoreTypeCreateInfo timelineCreateInfo = {};
//    timelineCreateInfo.sType            = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
//    timelineCreateInfo.pNext            = VK_NULL_HANDLE;
//    timelineCreateInfo.semaphoreType    = VK_SEMAPHORE_TYPE_TIMELINE;
//    timelineCreateInfo.initialValue     = 0;

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
//    semaphoreInfo.pNext = &timelineCreateInfo;
    semaphoreInfo.flags  = 0;

    // Create fences
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ ) {
        VK_CHECK(vkCreateSemaphore(vkContext.device, &semaphoreInfo, nullptr, &m_imgAvailableSemaphores[i]))
        VK_CHECK(vkCreateSemaphore(vkContext.device, &semaphoreInfo, nullptr, &m_renderCompleteSemaphores[i]))
        VK_CHECK(vkCreateFence(vkContext.device, &fenceInfo, nullptr, &m_inFlightFences[i]))
    }
}

/*
================================================================================
RenderBackend::DestroySyncObjects

DESCRIPTION:
Destroys the semaphores and fences.
================================================================================
*/
void RenderBackend::DestroySyncObjects() {

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i ) {
        vkDestroySemaphore(vkContext.device, m_imgAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(vkContext.device, m_renderCompleteSemaphores[i], nullptr);
        vkDestroyFence(vkContext.device, m_inFlightFences[i], nullptr);
    }
}

/*
================================================================================
RenderBackend::CreateQueryPool

DESCRIPTION:
Create Query Pool.
================================================================================
*/
void RenderBackend::CreateQueryPool() {
    VkQueryPoolCreateInfo createInfo = {};
    createInfo.sType        = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
    createInfo.queryType    = VK_QUERY_TYPE_TIMESTAMP;
    createInfo.queryCount   = NUM_TIMESTAMP_QUERIES;

    for (auto & queryPool : m_queryPools) {
        VK_CHECK(vkCreateQueryPool(vkContext.device, &createInfo, nullptr, &queryPool))
    }
}

/*
================================================================================
RenderBackend::DestroyQueryPool

DESCRIPTION:
Destroys Query Pool.
================================================================================
*/
void RenderBackend::DestroyQueryPool() {
    for (auto & queryPool : m_queryPools) {
        vkDestroyQueryPool(vkContext.device, queryPool, nullptr);
    }
}

/*
================================================================================
RenderBackend::CreateCommandPool

DESCRIPTION:
Creates the command pool.
================================================================================
*/
void RenderBackend::CreateCommandPool() {
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = static_cast<uint32_t>(vkContext.graphicsFamilyIdx);

    VK_CHECK(vkCreateCommandPool(vkContext.device, &createInfo, nullptr, &m_commandPool))
}

/*
================================================================================
RenderBackend::CreateCommandBuffer

DESCRIPTION:
Creates the command buffer.
================================================================================
*/
void RenderBackend::CreateCommandBuffer() {
    VkCommandBufferAllocateInfo cbAllocateInfo = {};
    cbAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cbAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cbAllocateInfo.commandPool = m_commandPool;
    cbAllocateInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    VK_CHECK(vkAllocateCommandBuffers(vkContext.device, &cbAllocateInfo, m_commandBuffers.data()))
}

/*
================================================================================
RenderBackend::CreateRenderTargets

DESCRIPTION:
Creates the render targets.
================================================================================
*/
void RenderBackend::CreateRenderTargets() {
    // Determine samples before creating depth
    VkImageFormatProperties fmtProps = {};
    vkGetPhysicalDeviceImageFormatProperties(
            vkContext.gpu.device,
            m_swapchainFormat,
            VK_IMAGE_TYPE_2D,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            0,
            &fmtProps);

    const int samples = vkConfig.MSAALevels;

    if (samples >= 16 && (fmtProps.sampleCounts & VK_SAMPLE_COUNT_16_BIT)) {
        vkContext.sampleCount = VK_SAMPLE_COUNT_16_BIT;
    } else if (samples >= 8 && (fmtProps.sampleCounts & VK_SAMPLE_COUNT_8_BIT)) {
        vkContext.sampleCount = VK_SAMPLE_COUNT_8_BIT;
    } else if (samples >= 4 && (fmtProps.sampleCounts & VK_SAMPLE_COUNT_4_BIT)) {
        vkContext.sampleCount = VK_SAMPLE_COUNT_4_BIT;
    } else if (samples >= 2 && (fmtProps.sampleCounts & VK_SAMPLE_COUNT_2_BIT)) {
        vkContext.sampleCount = VK_SAMPLE_COUNT_2_BIT;
    }

    // Select Depth Format
    {
        const int numFormats = 2;
//        VkFormat formats[numFormats] = {
//                VK_FORMAT_D32_SFLOAT_S8_UINT,
//                VK_FORMAT_D24_UNORM_S8_UINT
//        };
        VkFormat formats[numFormats] = {
                VK_FORMAT_D32_SFLOAT,
                VK_FORMAT_D24_UNORM_S8_UINT
        };
        vkContext.depthFormat = ChooseSupportedFormat(
                formats,
                numFormats,
                VK_IMAGE_TILING_OPTIMAL,
                VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

    ImageOpts depthOptions;
    depthOptions.format = FMT_DEPTH;
    depthOptions.width = static_cast<uint32_t>(window.width);
    depthOptions.height = static_cast<uint32_t>(window.height);
    depthOptions.numLevels = 1;
    depthOptions.samples = vkContext.sampleCount;

    globalImages->ScratchImage("_viewDepth", depthOptions);

    if (vkContext.sampleCount > VK_SAMPLE_COUNT_1_BIT ) {
        vkContext.superSampling = vkContext.gpu.features.sampleRateShading == VK_TRUE;

        VkImageCreateInfo createInfo = {};
        createInfo.sType            = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        createInfo.imageType        = VK_IMAGE_TYPE_2D;
        createInfo.format           = m_swapchainFormat;
        createInfo.extent.width     = m_swapchainExtent.width;
        createInfo.extent.height    = m_swapchainExtent.height;
        createInfo.extent.depth     = 1;
        createInfo.mipLevels        = 1;
        createInfo.arrayLayers      = 1;
        createInfo.samples          = vkContext.sampleCount;
        createInfo.tiling           = VK_IMAGE_TILING_OPTIMAL;
        createInfo.usage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                                    | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
                                    | VK_IMAGE_USAGE_SAMPLED_BIT
                                    | VK_IMAGE_USAGE_STORAGE_BIT;
        createInfo.initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;

        VK_CHECK(vkCreateImage(vkContext.device, &createInfo, nullptr, &m_currentImage))

        VmaAllocationCreateInfo vmaCreateInfo = {};
        vmaCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	    VK_CHECK(vmaCreateImage(
	            vmaAllocator,
	            &createInfo,
	            &vmaCreateInfo,
	            &m_currentImage,
	            &vmaAllocation,
	            &vmaAllocationInfo))


	    RVkCreateImageView(m_currentImage, m_swapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

/*
================================================================================
RenderBackend::DestroyRenderTargets

DESCRIPTION:
Destroys the render targets.
================================================================================
*/
void RenderBackend::DestroyRenderTargets() {
    vkDestroyImageView(vkContext.device, m_currentImageView, nullptr);
    vmaDestroyImage(vmaAllocator, m_currentImage, vmaAllocation);
    vmaAllocationInfo = VmaAllocationInfo();
    vmaAllocation = nullptr;
    m_currentImage = VK_NULL_HANDLE;
    m_currentImageView = VK_NULL_HANDLE;
}

/*
================================================================================
RenderBackend::CreateRenderPass

DESCRIPTION:
Creates a single render pass on start up.
TODO: Refacor to support creating multiple render passes on the fly.
================================================================================
*/
void RenderBackend::CreateRenderPass() {
    VkAttachmentDescription attachments[3];
    memset(attachments, 0, sizeof(attachments));

    const bool resolve = vkContext.sampleCount > VK_SAMPLE_COUNT_1_BIT;

    VkAttachmentDescription & colorAttachment = attachments[0];
    colorAttachment.format = m_swapchainFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription & depthAttachment = attachments[1];
    depthAttachment.format = vkContext.depthFormat;
    depthAttachment.samples = vkContext.sampleCount;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    if (resolve) {
        VkAttachmentDescription & resolveAttachment = attachments[2];
        resolveAttachment.format = m_swapchainFormat;
        resolveAttachment.samples = vkContext.sampleCount;
        resolveAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        resolveAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        resolveAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        resolveAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        resolveAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        resolveAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
    }

    VkAttachmentReference colorRef = {};
    colorRef.attachment = resolve ? 2 : 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthRef = {};
    depthRef.attachment = 1;
    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference resolveRef = {};
    resolveRef.attachment = 0;
    resolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;
    subpass.pDepthStencilAttachment = &depthRef;
    if (resolve) {
        subpass.pResolveAttachments = &resolveRef;
    }

//    VkSubpassDependency dependency{};
//    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//    dependency.dstSubpass = 0;
//    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//    dependency.srcAccessMask = 0;
//    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = resolve ? 3 : 2;
    renderPassCreateInfo.pAttachments = attachments;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 0;
//    renderPassCreateInfo.pDependencies = &dependency;

    VK_CHECK(vkCreateRenderPass(vkContext.device, &renderPassCreateInfo, nullptr, &vkContext.renderPass))
}

/*
================================================================================
RenderBackend::ClearView

DESCRIPTION:
Clears the view/screen with the specified color.
================================================================================
*/
void RenderBackend::ClearView(
        bool isColor,
        bool isDepth,
        bool isStencil,
        uint32_t stencilValue,
        float r,
        float g,
        float b,
        float a) {

    uint32_t numAttachments = 0;
    VkClearAttachment attachments[2];
    memset(attachments, 0, sizeof(attachments));
    VkCommandBuffer commandBuffer = m_commandBuffers[m_currentFrame];

    if (isColor) {
        VkClearAttachment & attachment = attachments[numAttachments++];
        attachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        attachment.colorAttachment = 0;
        VkClearColorValue & color = attachment.clearValue.color;
        color.float32[0] = r;
        color.float32[1] = g;
        color.float32[2] = b;
        color.float32[3] = a;
    }

    if (isDepth || isStencil ) {
        VkClearAttachment & attachment = attachments[numAttachments++];
        if (isDepth) {
            attachment.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        if (isStencil) {
            attachment.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        attachment.clearValue.depthStencil.depth = 1.0f;
        attachment.clearValue.depthStencil.stencil = stencilValue;
    }

    VkClearRect clearRect = {};
    clearRect.baseArrayLayer = 0;
    clearRect.layerCount = 1;
    clearRect.rect.extent = m_swapchainExtent;

    vkCmdClearAttachments(commandBuffer, numAttachments, attachments, 1, &clearRect);
}

/*
================================================================================
RenderBackend::StartFrame

DESCRIPTION:
Starts the frame drawing logic on the screen.
================================================================================
*/
void RenderBackend::StartFrame() {
    VK_CHECK(vkAcquireNextImageKHR(
            vkContext.device,
            m_swapchain,
            UINT64_MAX,
            m_imgAvailableSemaphores[m_currentFrame],
            VK_NULL_HANDLE,
            &m_currentSwapIdx));

    stagingManager.Flush();

    VkQueryPool queryPool = m_queryPools[m_currentFrame];
    std::array<uint64_t, NUM_TIMESTAMP_QUERIES> & results = m_queryResults[m_currentFrame];

    if (m_queryIndex[m_currentFrame] > 0 ) {
        vkGetQueryPoolResults(
                vkContext.device,
                queryPool, 0, 2,
                sizeof(results.data()),
                results.data(),
                sizeof(uint64_t),
                VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);

        const uint64_t gpuStart = results[0];
        const uint64_t gpuEnd = results[1];
        auto tick = static_cast<uint64_t>(
                1000000000 / vkContext.gpu.props.limits.timestampPeriod
        );

        m_pc.gpuMicroSec = ((gpuEnd - gpuStart) * 1000 * 1000) / tick;

        m_queryIndex[m_currentFrame] = 0;
    }

    VkCommandBuffer commandBuffer = m_commandBuffers[m_currentFrame];

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

    vkCmdResetQueryPool(commandBuffer, queryPool, 0, NUM_TIMESTAMP_QUERIES);

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = vkContext.renderPass;
    renderPassBeginInfo.framebuffer = m_frameBuffers[m_currentSwapIdx];
    renderPassBeginInfo.renderArea.extent = m_swapchainExtent;

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queryPool, m_queryIndex[m_currentFrame]++);
}

/*
================================================================================
RenderBackend::EndFrame

DESCRIPTION:
Ends the frame drawing logic and submits the result to the queue.
================================================================================
*/
void RenderBackend::EndFrame() {
    VkCommandBuffer commandBuffer = m_commandBuffers[m_currentFrame];

    vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_queryPools[ m_currentFrame ], m_queryIndex[ m_currentFrame ]++);
    vkCmdEndRenderPass(commandBuffer);

    // Transition our swap image to present.
    // Do this instead of having the renderpass do the transition
    // so we can take advantage of the general layout to avoid
    // additional image barriers.
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_swapchainImages[m_currentSwapIdx];
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = 0;

    vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0, 0, nullptr, 0, nullptr, 1, &barrier);

    VK_CHECK(vkEndCommandBuffer(commandBuffer))
    m_commandBufferRecorded[ m_currentFrame ] = true;

    VkSemaphore * waitSemaphore = &m_imgAvailableSemaphores[m_currentFrame];
    VkSemaphore * signalSemaphore = &m_renderCompleteSemaphores[m_currentFrame];

    VkPipelineStageFlags dstStageMask = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphore;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphore;
    submitInfo.pWaitDstStageMask = &dstStageMask;

    VK_CHECK(vkQueueSubmit(vkContext.graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]));

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_swapchain;
    presentInfo.pImageIndices = &m_currentSwapIdx;

    VK_CHECK(vkQueuePresentKHR(vkContext.presentQueue, &presentInfo));

    m_counter++;
    m_currentFrame = m_counter % MAX_FRAMES_IN_FLIGHT;
}

/*
================================================================================
RenderBackend::Execute

DESCRIPTION:
Executes the render commands.
================================================================================
*/

void RenderBackend::Execute() {
    StartFrame();

    //TODO: Add loop and a list of commands to execute.
    DrawView();

    EndFrame();
}

/*
================================================================================
RenderBackend::DrawView

DESCRIPTION:
Draws the view onto the screen.
================================================================================
*/

void RenderBackend::DrawView() {
    // TODO: Set proper viewport and scissor settings.
//    // set the window clipping
//    Viewport( m_viewDef->viewport.x1,
//                 m_viewDef->viewport.y1,
//                 m_viewDef->viewport.x2 + 1 - m_viewDef->viewport.x1,
//                 m_viewDef->viewport.y2 + 1 - m_viewDef->viewport.y1 );
//
//    // the scissor may be smaller than the viewport for subviews
//    Scissor( m_viewDef->viewport.x1 + m_viewDef->scissor.x1,
//                m_viewDef->viewport.y1 + m_viewDef->scissor.y1,
//                m_viewDef->scissor.x2 + 1 - m_viewDef->scissor.x1,
//                m_viewDef->scissor.y2 + 1 - m_viewDef->scissor.y1 );
//    m_currentScissor = m_viewDef->scissor;
//
//    // ensures that depth writes are enabled for the depth clear
//    State( GLS_DEFAULT | GLS_CULL_FRONTSIDED, true );
//
//    // Clear the depth buffer and clear the stencil to 128 for stencil shadows as well as gui masking
//    ClearView( false, true, true, STENCIL_SHADOW_TEST_VALUE, 0.0f, 0.0f, 0.0f, 0.0f );

    int left, top;
    SDL_GetWindowPosition(window.handle, &left, &top);

    Viewport(left, top, window.width, window.height);
    Scissor(left, top, window.width, window.height);

    // Clear depth and stencil buffer.
//    ClearView(false, true, true, STENCIL_SHADOW_TEST_VALUE, 0.0f, 0.0f, 0.0f, 0.0f);

    // Clear color buffer.
    ClearView(true, false, false, STENCIL_SHADOW_TEST_VALUE, 0.0f, 0.0f, 0.0f, 0.0f);
}

/*
================================================================================
RenderBackend::Scissor

DESCRIPTION:
Updates the Scissors/Clipping rectangle transformation parameters.
================================================================================
*/
void RenderBackend::Scissor(int x /* left*/, int y /* bottom */, int w, int h) {
    VkCommandBuffer commandBuffer = m_commandBuffers[m_currentFrame];
    VkRect2D scissor;
    scissor.offset.x = x;
    scissor.offset.y = y;
    scissor.extent.width = static_cast<uint32_t>(w);
    scissor.extent.height = static_cast<uint32_t>(h);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

/*
================================================================================
RenderBackend::Viewport

DESCRIPTION:
Updates the viewport transformation parameters.
================================================================================
*/
void RenderBackend::Viewport(int x /* left */, int y /* bottom */, int w, int h) {
    VkCommandBuffer commandBuffer = m_commandBuffers[m_currentFrame];
    VkViewport viewport;
    viewport.x = static_cast<float>(x);
    viewport.y = static_cast<float>(y);
    viewport.width = static_cast<float>(w);
    viewport.height = static_cast<float>(h);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
}

/*
================================================================================
RenderBackend::SwapBuffers

DESCRIPTION:
Swaps the front and back buffers.
================================================================================
*/
void RenderBackend::SwapBuffers() {
    m_counter++;
    m_currentFrame = m_counter % MAX_FRAMES_IN_FLIGHT;

    if (m_commandBufferRecorded[m_currentFrame] == false) {
        return;
    }

    VK_CHECK(vkWaitForFences(vkContext.device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX))

    VK_CHECK(vkResetFences(vkContext.device, 1, &m_inFlightFences[m_currentFrame]))
    m_commandBufferRecorded[m_currentFrame] = false;
}
