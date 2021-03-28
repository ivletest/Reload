#include "RenderBackend.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "../Config.h"
#include "Staging.h"
#include "Image.h"

VulkanConfig        vkConfig;
WindowConfig        windowConfig;
VkContext           vkContext;

/*
================================================================================
Defines
================================================================================
*/
#define ID_VK_ERROR_STRING( x ) case static_cast<int>( x ): return #x

/*
================================================================================
Constants
================================================================================
*/
const char * VK_LAYER_KHRONOS_VALIDATION_NAME = "VK_LAYER_KHRONOS_validation";

/*
================================================================================
Static properties
================================================================================
*/
static VkDebugReportCallbackEXT s_debugReportCallback = VK_NULL_HANDLE;

static const char * g_debugExtensions[1] = {
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME
};

static const char * g_deviceExtensions[1] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static const char * g_validationLayers[1] = {
        VK_LAYER_KHRONOS_VALIDATION_NAME
};

/*
================================================================================
Vk_ErrorToString

DESCRIPTION:
Converts VkResult enum value to string and returns it.
================================================================================
*/
[[maybe_unused]] const char * Vk_ErrorToString(VkResult result) {
    switch (result) {
        ID_VK_ERROR_STRING( VK_SUCCESS );
        ID_VK_ERROR_STRING( VK_NOT_READY );
        ID_VK_ERROR_STRING( VK_TIMEOUT );
        ID_VK_ERROR_STRING( VK_EVENT_SET );
        ID_VK_ERROR_STRING( VK_EVENT_RESET );
        ID_VK_ERROR_STRING( VK_INCOMPLETE );
        ID_VK_ERROR_STRING( VK_ERROR_OUT_OF_HOST_MEMORY );
        ID_VK_ERROR_STRING( VK_ERROR_OUT_OF_DEVICE_MEMORY );
        ID_VK_ERROR_STRING( VK_ERROR_INITIALIZATION_FAILED );
        ID_VK_ERROR_STRING( VK_ERROR_DEVICE_LOST );
        ID_VK_ERROR_STRING( VK_ERROR_MEMORY_MAP_FAILED );
        ID_VK_ERROR_STRING( VK_ERROR_LAYER_NOT_PRESENT );
        ID_VK_ERROR_STRING( VK_ERROR_EXTENSION_NOT_PRESENT );
        ID_VK_ERROR_STRING( VK_ERROR_FEATURE_NOT_PRESENT );
        ID_VK_ERROR_STRING( VK_ERROR_INCOMPATIBLE_DRIVER );
        ID_VK_ERROR_STRING( VK_ERROR_TOO_MANY_OBJECTS );
        ID_VK_ERROR_STRING( VK_ERROR_FORMAT_NOT_SUPPORTED );
        ID_VK_ERROR_STRING( VK_ERROR_SURFACE_LOST_KHR );
        ID_VK_ERROR_STRING( VK_ERROR_NATIVE_WINDOW_IN_USE_KHR );
        ID_VK_ERROR_STRING( VK_SUBOPTIMAL_KHR );
        ID_VK_ERROR_STRING( VK_ERROR_OUT_OF_DATE_KHR );
        ID_VK_ERROR_STRING( VK_ERROR_INCOMPATIBLE_DISPLAY_KHR );
        ID_VK_ERROR_STRING( VK_ERROR_VALIDATION_FAILED_EXT );
        ID_VK_ERROR_STRING( VK_ERROR_INVALID_SHADER_NV );
        default: return "UNKNOWN";
    }
}

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

    std::vector<VkLayerProperties> instanceLayers;
    instanceLayers.resize(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, instanceLayers.data());

    bool layerFound = false;
    for (auto enabledValidationLayer : g_validationLayers) {
        for (int j = 0; j < (int)layerCount; j++) {
            VkLayerProperties layerProps = instanceLayers[j];
            if (!strcmp(enabledValidationLayer, layerProps.layerName)) {
                layerFound = true;
                break;
            }
        }
        if(!layerFound) {
            SDL_LogCritical(
                    LOG_SYSTEM,
                    "Validation layer '%s' not supported by graphics device.",
                    enabledValidationLayer);
            exit(1);
        }
    }
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

/*
================================================================================
CreateDebugReportCallback

DESCRIPTION:
Creates the debug report callback.
================================================================================
*/
static void CreateDebugReportCallback(VkInstance instance) {
    VkDebugReportCallbackCreateInfoEXT callbackInfo = {};
    callbackInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    callbackInfo.flags = VK_DEBUG_REPORT_DEBUG_BIT_EXT
                       | VK_DEBUG_REPORT_WARNING_BIT_EXT
                       | VK_DEBUG_REPORT_ERROR_BIT_EXT;
    callbackInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT) DebugCallback;

    PFN_vkCreateDebugReportCallbackEXT func;
    func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT" );

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
    VK_CHECK(vkCreatePipelineCache(vkContext.device, &pipelineCacheCreateInfo, nullptr, &vkContext.pipelineCache))
}

/*
================================================================================
ChooseSurfaceFormat
DESCRIPTION:
Chooses the appropriate surface format.
================================================================================
*/
static VkSurfaceFormatKHR ChooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> & formats) {
    VkSurfaceFormatKHR result;

    if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
        result.format = VK_FORMAT_B8G8R8A8_UNORM;
        result.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        return result;
    }

    for (auto & fmt : formats) {
        if (fmt.format == VK_FORMAT_B8G8R8A8_SRGB
        && fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return fmt;
        }
    }

    return formats[0];
}

/*
================================================================================
ChoosePresentMode

DESCRIPTION:
Chooses the appropriate presentation mode.
================================================================================
*/
static VkPresentModeKHR ChoosePresentMode(std::vector<VkPresentModeKHR> & modes) {
    for (auto & mode : modes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return mode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

/*
================================================================================
ChooseSupportedFormat

DESCRIPTION:
Chooses the supported depth format.
================================================================================
*/
static VkFormat ChooseSupportedFormat(
        VkPhysicalDevice physicalDevice,
        const VkFormat * formats,
        int numFormats,
        VkImageTiling tiling,
        VkFormatFeatureFlags features) {
    for ( int i = 0; i < numFormats; ++i ) {
        VkFormat format = formats[ i ];

        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if ((props.linearTilingFeatures & features) == features
            && (tiling == VK_IMAGE_TILING_LINEAR || tiling == VK_IMAGE_TILING_OPTIMAL)) {
            return format;
        }
    }

    SDL_LogError(LOG_RENDER, "Failed to find a supported format.");

    return VK_FORMAT_UNDEFINED;
}

/*
================================================================================
RenderBackend::GetPhysicalDeviceInfo

DESCRIPTION:
Gets the physical graphics device information required to rate and check whether
it is capable of running the program.

RETURNS:
GPUInfo struct containing the required information.
================================================================================
*/

GPUInfo RenderBackend::GetPhysicalDeviceInfo(VkPhysicalDevice device) {
    GPUInfo gpu;
    gpu.device = device;

    {
        uint32_t numQueues = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(gpu.device, &numQueues, nullptr);
        VK_VALIDATE(numQueues > 0, "No queue families supported by device")

        gpu.queueFamilyProps.resize(numQueues);
        vkGetPhysicalDeviceQueueFamilyProperties(gpu.device, &numQueues, gpu.queueFamilyProps.data());
    }

    {
        uint32_t numExtensions;
        VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.device, nullptr, &numExtensions, nullptr))
        VK_VALIDATE(numExtensions > 0, "No extensions supported by device" )

        gpu.extensionProps.resize(numExtensions);
        VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.device, nullptr, &numExtensions, gpu.extensionProps.data()))
    }

    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu.device, m_surface, &gpu.surfaceCaps))

    {
        uint32_t numFormats;
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.device, m_surface, &numFormats, nullptr))
        VK_VALIDATE(numFormats > 0, "No surface formats supported by device")

        gpu.surfaceFormats.resize(numFormats);
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.device, m_surface, &numFormats, gpu.surfaceFormats.data()))
    }

    {
        uint32_t numPresentModes;
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu.device, m_surface, &numPresentModes, nullptr))
        VK_VALIDATE(numPresentModes > 0, "No present modes supported by device")

        gpu.presentModes.resize(numPresentModes);
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu.device, m_surface, &numPresentModes, gpu.presentModes.data()))
    }

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
CheckPhysicalDeviceExtensionSupport

DESCRIPTION:
Checks whether the gpu supports all the extensions required to run the program.
================================================================================
*/
static bool CheckPhysicalDeviceExtensionSupport(GPUInfo & gpu, std::vector<const char *> & requiredExt) {
    size_t extensionPropsCount = gpu.extensionProps.size();
    size_t required = requiredExt.size();
    size_t available = 0;

    for (size_t i = 0; i < required; ++i) {
        for (size_t j = 0; j < extensionPropsCount; ++j ) {
            if (strcmp(requiredExt[i], gpu.extensionProps[j].extensionName ) == 0) {
                available++;
                break;
            }
        }
    }

    return available == required;
}

/*
================================================================================
RenderBackend::RenderBackend

DESCRIPTION:
The render backend's constructor.
================================================================================
*/
RenderBackend::RenderBackend() {
    m_window = nullptr;
    m_windowWidth = m_windowHeight = 0;
    m_physicalDevice = VK_NULL_HANDLE;
    m_instance = VK_NULL_HANDLE;
    m_device = VK_NULL_HANDLE;

    m_debugCallbackHandle = VK_NULL_HANDLE;
    m_debugUtilsCallbackHandle = VK_NULL_HANDLE;

    m_queueIndex = 0;
    m_queue = VK_NULL_HANDLE;
    m_allocator = VK_NULL_HANDLE;
    m_msaaSamples = VK_SAMPLE_COUNT_32_BIT;
    m_presentMode = {};

    m_surface = VK_NULL_HANDLE;

    ClearContext();
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
    vkContext.gpu = GPUInfo();
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
//    m_counter = 0;
//    m_currentFrameData = 0;

    m_instance = VK_NULL_HANDLE;
    m_physicalDevice = VK_NULL_HANDLE;

    s_debugReportCallback = VK_NULL_HANDLE;
    m_instanceExtensions.clear();
    m_deviceExtensions.clear();
    m_validationLayers.clear();

    m_surface = VK_NULL_HANDLE;
    m_presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

//    m_fullscreen = 0;
//    m_swapChain = VK_NULL_HANDLE;
//    m_swapChainFormat = VK_FORMAT_UNDEFINED;
//    m_currentSwapIndex = 0;
//    m_msaaImage = VK_NULL_HANDLE;
//    m_msaaImageView = VK_NULL_HANDLE;
//    m_commandPool = VK_NULL_HANDLE;
//
//    m_swapChainImages.Zero();
//    m_swapChainViews.Zero();
//    m_frameBuffers.Zero();
//
//    m_commandBuffers.Zero();
//    m_commandBufferFences.Zero();
//    m_commandBufferRecorded.Zero();
//    m_acquireSemaphores.Zero();
//    m_renderCompleteSemaphores.Zero();

//    m_queryIndex.Zero();
//    for ( int i = 0; i < NUM_FRAME_DATA; ++i ) {
//        m_queryResults[ i ].Zero();
//    }
//    m_queryPools.Zero();
}

/*
================================================================================
RenderBackend::Init

DESCRIPTION:
Initializes the render backend.
================================================================================
*/
void RenderBackend::Init() {
    vkConfig      = Config::Get()->VulkanConfiguration();
    windowConfig  = Config::Get()->WindowConfiguration();

    CreateWindow();
    CreateInstance();
    CreateSurface();
    SelectPhysicalDevice();
    CreateLogicalDeviceAndQueues();
    CreateSemaphores();
    CreateQueryPool();
    CreateCommandPool();
    CreateCommandBuffer();
    CreateMemoryAllocator();
    staging.Init();
    CreateSwapChain();
    CreateRenderTargets();
    CreateRenderPass();
    CreatePipelineCache();
    CreateFrameBuffers();
}

/*
================================================================================
RenderBackend::Shutdown

DESCRIPTION:
Shuts down the render backend.
================================================================================
*/
void RenderBackend::Shutdown() {
    DestroySwapChain();

    vkFreeCommandBuffers(vkContext.device, m_commandPool, NUM_FRAME_DATA, m_commandBuffers);

    for (auto & m_commandBufferFence : m_commandBufferFences) {
        vkDestroyFence(vkContext.device, m_commandBufferFence, nullptr);
    }

    vkDestroyCommandPool(vkContext.device, m_commandPool, nullptr);

    for (auto & m_queryPool : m_queryPools) {
        vkDestroyQueryPool(vkContext.device, m_queryPool, nullptr );
    }

    for (int i = 0; i < (int)NUM_FRAME_DATA; ++i ) {
        vkDestroySemaphore(vkContext.device, m_acquireSemaphores[i], nullptr);
        vkDestroySemaphore(vkContext.device, m_renderCompleteSemaphores[i], nullptr);
    }

    if (vkConfig.enableDebugLayer) {
        DestroyDebugReportCallback(m_instance);
    }

    vkDestroyDevice(vkContext.device, nullptr);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyInstance(m_instance, nullptr);

    ClearContext();
    Clear();

    SDL_DestroyWindow(m_window);
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
RenderBackend::CreateWindow

DESCRIPTION:
Configures and creates the SDL Window.
================================================================================
*/
void RenderBackend::CreateWindow() {
    uint32_t windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN;

    if (windowConfig.allowHighDpi) {
        windowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
        SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    }
    if (windowConfig.isFullScreen) {
        windowFlags |= SDL_WINDOW_FULLSCREEN;
    }

    m_window = SDL_CreateWindow(
            windowConfig.title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            (int)windowConfig.width,
            (int)windowConfig.height,
            windowFlags);

    if (!m_window) {
        SDL_LogCritical(LOG_VIDEO, "Failed to open window: %s\n", SDL_GetError());
        exit(1);
    }
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

    uint32_t apiVer = VK_MAKE_VERSION(vkConfig.apiVersion.major,
                                      vkConfig.apiVersion.minor,
                                      vkConfig.apiVersion.patch);

    VkApplicationInfo appInfo  = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = vkConfig.programName;
    appInfo.applicationVersion = appVer;
    appInfo.pEngineName = vkConfig.engineName;
    appInfo.engineVersion = engineVer;
    appInfo.apiVersion = apiVer;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    m_instanceExtensions.clear();
    m_deviceExtensions.clear();
    m_validationLayers.clear();

    uint32_t extensionCount;
    SDL_Vulkan_GetInstanceExtensions(m_window, &extensionCount, nullptr);
    m_instanceExtensions.resize(extensionCount);
    SDL_Vulkan_GetInstanceExtensions(m_window, &extensionCount, m_instanceExtensions.data());

    for (auto & deviceExtension : g_deviceExtensions) {
        m_deviceExtensions.push_back(deviceExtension);
    }

    if (vkConfig.enableValidationLayers) {
        for (auto & debugExtension : g_debugExtensions) {
            m_instanceExtensions.push_back(debugExtension);
        }

        for (auto & validationLayer : g_validationLayers) {
            m_validationLayers.push_back(validationLayer);
        }

        CheckValidationLayers();
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(m_instanceExtensions.size());
    createInfo.ppEnabledExtensionNames = m_instanceExtensions.data();
    createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
    createInfo.ppEnabledLayerNames = m_validationLayers.data();

    VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_instance))

    if (vkConfig.enableDebugLayer) {
        CreateDebugReportCallback(m_instance);
    }
}

/*
================================================================================
RenderBackend::CreateSurface

DESCRIPTION:
Configures and creates The Vulkan surface to be drawn to.
================================================================================
*/
void RenderBackend::CreateSurface() {
    if (!SDL_Vulkan_CreateSurface(m_window, m_instance, &m_surface)) {
        SDL_LogCritical(LOG_VIDEO, "Cannot create Vulkan window surface");
        exit(1);
    }
}

/*
================================================================================
RenderBackend::CreateInstance

DESCRIPTION:
Selects the suitable graphics device.
================================================================================
*/
void RenderBackend::SelectPhysicalDevice() {
    uint32_t deviceCount = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr))
    VK_VALIDATE(deviceCount > 0, "Can't find Vulkan enabled device.")

    std::vector<VkPhysicalDevice> devices(deviceCount);
    VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data()))

    int bestScore = -1;
    GPUInfo bestGpu;

    for (VkPhysicalDevice &device : devices) {
        GPUInfo gpu = GetPhysicalDeviceInfo(device);

        if (gpu.score >= bestScore) {
            bestScore = gpu.score;
            bestGpu = gpu;
        }
    }

    if (!CheckPhysicalDeviceExtensionSupport(bestGpu, m_deviceExtensions)) {
        SDL_LogCritical(LOG_SYSTEM, "GPU doesn't support required extensions");
        exit(1);
    }

    if (bestGpu.surfaceFormats.empty()) {
        SDL_LogCritical(LOG_SYSTEM, "GPU doesn't support surface formats.");
        exit(1);
    }

    if (bestGpu.presentModes.empty()) {
        SDL_LogCritical(LOG_SYSTEM, "GPU doesn't support present modes.");
        exit(1);
    }

    size_t numQueueFamilyProps = bestGpu.queueFamilyProps.size();
    // Find graphics queue family
    for (uint32_t j = 0; j < numQueueFamilyProps; ++j) {
        VkQueueFamilyProperties & props = bestGpu.queueFamilyProps[j];

        if (props.queueCount == 0) {
            continue;
        }

        if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            vkContext.queueFamilies.graphicsFamily = j;
            break;
        }
    }

    // Find present queue family
    for (uint32_t j = 0; j < numQueueFamilyProps; ++j) {
        VkQueueFamilyProperties & props = bestGpu.queueFamilyProps[j];

        if (props.queueCount == 0) {
            continue;
        }

        VkBool32 supportsPresent = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(bestGpu.device, j, m_surface, &supportsPresent);

        if (supportsPresent) {
            vkContext.queueFamilies.presentFamily = j;
            break;
        }
    }

    if (!vkContext.queueFamilies.AreSupported()) {
        // If we can't render or present, just bail.
        SDL_LogCritical(LOG_SYSTEM, "GPU doesn't meet the requirements");
        exit(1);
    }

    m_physicalDevice = bestGpu.device;
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
    uint32_t graphicsFamilyIdx = vkContext.queueFamilies.graphicsFamily.value();
    uint32_t presentFamilyIdx = vkContext.queueFamilies.presentFamily.value();
    uint32_t queueFamilyIndices[2] = {graphicsFamilyIdx, presentFamilyIdx };

    std::vector<VkDeviceQueueCreateInfo> queueInfos;
    const float queuePriority = 1.0f;

    for (auto & queueFamilyIndex : queueFamilyIndices) {
        VkDeviceQueueCreateInfo queueInfo = {};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = queueFamilyIndex;
        queueInfo.queueCount  = 1;
        queueInfo.pQueuePriorities = &queuePriority;

        queueInfos.push_back(queueInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.textureCompressionBC = VK_TRUE;
    deviceFeatures.imageCubeArray = VK_TRUE;
    deviceFeatures.depthClamp = VK_TRUE;
    deviceFeatures.depthBiasClamp = VK_TRUE;
    deviceFeatures.depthBounds = vkContext.gpu.features.depthBounds;
    deviceFeatures.fillModeNonSolid  = VK_TRUE;

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

    VK_CHECK(vkCreateDevice(m_physicalDevice, &deviceInfo, nullptr, &vkContext.device))

    vkGetDeviceQueue(vkContext.device, graphicsFamilyIdx, 0, &vkContext.graphicsQueue);
    vkGetDeviceQueue(vkContext.device, presentFamilyIdx , 0, &vkContext.presentQueue);
}

/*
================================================================================
RenderBackend::CreateSemaphores

DESCRIPTION:
Create semaphores for image acquisition and rendering completion.
================================================================================
*/
void RenderBackend::CreateSemaphores() {
//    VkSemaphoreTypeCreateInfo timelineCreateInfo = {};
//    timelineCreateInfo.sType            = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
//    timelineCreateInfo.pNext            = VK_NULL_HANDLE;
//    timelineCreateInfo.semaphoreType    = VK_SEMAPHORE_TYPE_TIMELINE;
//    timelineCreateInfo.initialValue     = 0;

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
//    semaphoreCreateInfo.pNext = &timelineCreateInfo;
    semaphoreCreateInfo.flags  = 0;

    for (uint32_t i = 0; i < NUM_FRAME_DATA; ++i ) {
        VK_CHECK(vkCreateSemaphore(vkContext.device, &semaphoreCreateInfo, nullptr, &m_acquireSemaphores[i]))
        VK_CHECK(vkCreateSemaphore(vkContext.device, &semaphoreCreateInfo, nullptr, &m_renderCompleteSemaphores[i]))
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

    for (auto & m_queryPool : m_queryPools) {
        VK_CHECK(vkCreateQueryPool(vkContext.device, &createInfo, nullptr, &m_queryPool))
    }
}

/*
================================================================================
RenderBackend::CreateCommandPool

DESCRIPTION:
Create command pool.
================================================================================
*/
void RenderBackend::CreateCommandPool() {
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType             = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags             = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex  = vkContext.queueFamilies.graphicsFamily.value();

    VK_CHECK(vkCreateCommandPool(vkContext.device, &createInfo, nullptr, &m_commandPool))
}

/*
================================================================================
RenderBackend::CreateCommandBuffer

DESCRIPTION:
Create command buffer.
================================================================================
*/
void RenderBackend::CreateCommandBuffer() {
    VkCommandBufferAllocateInfo cbAllocateInfo = {};
    cbAllocateInfo.sType                = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cbAllocateInfo.level                = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cbAllocateInfo.commandPool          = m_commandPool;
    cbAllocateInfo.commandBufferCount   = NUM_FRAME_DATA;

    VK_CHECK(vkAllocateCommandBuffers(vkContext.device, &cbAllocateInfo, m_commandBuffers))

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    for (auto & m_commandBufferFence : m_commandBufferFences) {
        VK_CHECK(vkCreateFence(vkContext.device, &fenceCreateInfo, nullptr, &m_commandBufferFence))
    }
}

/*
================================================================================
RenderBackend::CreateMemoryAllocator

DESCRIPTION:
Creates the Vulkan memory allocator.
================================================================================
*/
void RenderBackend::CreateMemoryAllocator() {
    VmaAllocatorCreateInfo createInfo = {};
    createInfo.instance = m_instance;
    createInfo.physicalDevice = m_physicalDevice;
    createInfo.device = vkContext.device;
    createInfo.preferredLargeHeapBlockSize = vkConfig.deviceLocalMemoryMB * 1024 * 1024;

    vmaCreateAllocator(&createInfo, &m_allocator);
}

/*
================================================================================
RenderBackend::DestroyMemoryAllocator

DESCRIPTION:
Destroys the Vulkan memory allocator.
================================================================================
*/
void RenderBackend::DestroyMemoryAllocator() {
    vmaDestroyAllocator(m_allocator);
}

/*
================================================================================
RenderBackend::ChooseSurfaceExtent

DESCRIPTION:
Chooses the appropriate surface extent.
================================================================================
*/
VkExtent2D RenderBackend::ChooseSurfaceExtent(VkSurfaceCapabilitiesKHR & caps) {

    if (caps.currentExtent.width != UINT32_MAX) {
        return caps.currentExtent;
    } else {
        int width, height;
        SDL_GetWindowSize(m_window, &width, &height);

        VkExtent2D extent;
        extent.width = std::max(
                caps.minImageExtent.width,
                std::min(caps.maxImageExtent.width, static_cast<uint32_t>(width)));
        extent.height = std::max(
                caps.minImageExtent.height,
                std::min(caps.maxImageExtent.height, static_cast<uint32_t>(height)));

        return extent;
    }
}

/*
================================================================================
RenderBackend::CreateSwapChain

DESCRIPTION:
Creates the swap chain.
================================================================================
*/
void RenderBackend::CreateSwapChain() {
    GPUInfo & gpu = vkContext.gpu;
    VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(gpu.surfaceFormats);
    VkPresentModeKHR presentMode = ChoosePresentMode(gpu.presentModes);
    VkExtent2D extent = ChooseSurfaceExtent(gpu.surfaceCaps);

    VkSwapchainCreateInfoKHR info = {};
    info.sType              = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface            = m_surface;
    info.minImageCount      = NUM_FRAME_DATA > gpu.surfaceCaps.maxImageCount
                              ? NUM_FRAME_DATA
                              : gpu.surfaceCaps.maxImageCount;
    info.imageFormat        = surfaceFormat.format;
    info.imageColorSpace    = surfaceFormat.colorSpace;
    info.imageExtent        = extent;
    info.imageArrayLayers   = 1; // Unless developing a stereoscopic 3D application, this should always be 1.
    info.imageUsage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                            | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    uint32_t graphicsFamilyIdx = vkContext.queueFamilies.graphicsFamily.value();
    uint32_t presentFamilyIdx = vkContext.queueFamilies.presentFamily.value();

    if (graphicsFamilyIdx != presentFamilyIdx) {
        uint32_t indices[] = { graphicsFamilyIdx, presentFamilyIdx };

        info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        info.queueFamilyIndexCount = 2;
        info.pQueueFamilyIndices = indices;
    } else {
        info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    info.preTransform   = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.presentMode    = presentMode;
    info.clipped        = VK_TRUE;
    info.oldSwapchain   = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(vkContext.device, &info, nullptr, &m_swapChain))

    m_swapChainFormat = surfaceFormat.format;
    m_presentMode = presentMode;
    m_swapChainExtent = extent;
    m_fullscreen = windowConfig.isFullScreen;

    uint32_t numImages = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(vkContext.device, m_swapChain, &numImages, nullptr))
    VK_VALIDATE(numImages > 0, "vkGetSwapchainImagesKHR returned a zero image count.")

    VK_CHECK(vkGetSwapchainImagesKHR(vkContext.device, m_swapChain, &numImages, m_swapChainImages))

    for (uint32_t i = 0; i < NUM_FRAME_DATA; ++i ) {
        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = m_swapChainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = m_swapChainFormat;
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

        VK_CHECK(vkCreateImageView(vkContext.device, &imageViewCreateInfo, nullptr, &m_swapChainViews[i]))
    }
}

/*
================================================================================
RenderBackend::DestroySwapChain

DESCRIPTION:
Destroys the swap chain.
================================================================================
*/
void RenderBackend::DestroySwapChain() {
    for (auto &swapchainView : m_swapChainViews) {
        vkDestroyImageView(vkContext.device, swapchainView, nullptr);
    }

    vkDestroySwapchainKHR(vkContext.device, m_swapChain, nullptr);
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
            m_physicalDevice,
            m_swapChainFormat,
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
        VkFormat formats[] = {
                VK_FORMAT_D32_SFLOAT_S8_UINT,
                VK_FORMAT_D24_UNORM_S8_UINT
        };
        vkContext.depthFormat = ChooseSupportedFormat(
                m_physicalDevice,
                formats,
                3,
                VK_IMAGE_TILING_OPTIMAL,
                VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
    }

    int windowWidth, windowHeight;
    SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);

    ImageOptions depthOptions;
    depthOptions.format = vkContext.depthFormat;
    depthOptions.width = static_cast<uint32_t>(windowWidth);
    depthOptions.height = static_cast<uint32_t>(windowHeight);
    depthOptions.mipLevels = 1;
    depthOptions.samples = vkContext.sampleCount;

//    globalImages->ScratchImage( "_viewDepth", depthOptions );

    if (vkContext.sampleCount > VK_SAMPLE_COUNT_1_BIT ) {
        vkContext.superSampling = vkContext.gpu.features.sampleRateShading == VK_TRUE;

        VkImageCreateInfo createInfo = {};
        createInfo.sType            = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        createInfo.imageType        = VK_IMAGE_TYPE_2D;
        createInfo.format           = m_swapChainFormat;
        createInfo.extent.width     = m_swapChainExtent.width;
        createInfo.extent.height    = m_swapChainExtent.height;
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

        VK_CHECK(vkCreateImage(vkContext.device, &createInfo, nullptr, &m_msaaImage))

        VmaAllocationCreateInfo vmaCreateInfo = {};
        vmaCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	    VK_CHECK(vmaCreateImage(
	            m_allocator,
	            &createInfo,
	            &vmaCreateInfo,
	            &m_msaaImage,
	            &m_msaaVmaAllocation,
	            &m_msaaAllocation))

        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.format = m_swapChainFormat;
        viewInfo.image = m_msaaImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(vkContext.device, &viewInfo, nullptr, &m_msaaImageView))
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

}

/*
================================================================================
RenderBackend::CreateRenderPass

DESCRIPTION:
Creates a render pass.
================================================================================
*/
void RenderBackend::CreateRenderPass() {

}

/*
================================================================================
RenderBackend::CreateFrameBuffers

DESCRIPTION:
Creates frame buffers.
================================================================================
*/
void RenderBackend::CreateFrameBuffers() {

}

/*
================================================================================
RenderBackend::DestroyFrameBuffers

DESCRIPTION:
Destroys frame buffers.
================================================================================
*/
void RenderBackend::DestroyFrameBuffers() {

}

void RenderBackend::PrepareScene() {
	int windowWidth, windowHeight;
	SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);
}

void RenderBackend::PresentScene() {
	// glClearColor(
	// 	colorNorm[96], 
	// 	colorNorm[128], 
	// 	colorNorm[255], 
	// 	colorNorm[255]);
	// glClear(GL_COLOR_BUFFER_BIT);
	/* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
	// nk_sdl_render(NK_ANTI_ALIASING_ON);
    //	SDL_GL_SwapWindow(window);
}