#include "RenderBackend.h"


#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "../Config.h"

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

static const int g_numDebugExtensions = 1;
static const char * g_debugExtensions[g_numDebugExtensions] = {
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME
};

static const int g_numDeviceExtensions = 1;
static const char * g_deviceExtensions[g_numDeviceExtensions] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static const int g_numValidationLayers = 1;
static const char * g_validationLayers[g_numValidationLayers] = {
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
Vk_CheckValidationLayers

DESCRIPTION:
Checks whether the graphic device supports the validation layers selected.
If the validation fails it exits the application.
================================================================================
*/
static void Vk_CheckValidationLayers() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    List<VkLayerProperties> instanceLayers;
    instanceLayers.SetCount((int)layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, instanceLayers.Data());

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
Logs the debug info message in the specifield format.
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
Vk_CreateDebugReportCallback

DESCRIPTION:
Creates the debug report callback.
================================================================================
*/
static void Vk_CreateDebugReportCallback(VkInstance instance) {
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
Vk_DestroyDebugReportCallback
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

        gpu.queueFamilyProps.SetCount((int)numQueues);
        vkGetPhysicalDeviceQueueFamilyProperties(gpu.device, &numQueues, gpu.queueFamilyProps.Data());
        VK_VALIDATE(numQueues > 0, "No queue families supported by device")
    }

    {
        uint32_t numExtensions;
        VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.device, nullptr, &numExtensions, nullptr))
        VK_VALIDATE(numExtensions > 0, "No extensions supported by device" )

        gpu.extensionProps.SetCount((int)numExtensions);
        VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.device, nullptr, &numExtensions, gpu.extensionProps.Data()))
        VK_VALIDATE(numExtensions > 0, "No extensions supported by device")
    }

    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu.device, m_surface, &gpu.surfaceCaps))

    {
        uint32_t numFormats;
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.device, m_surface, &numFormats, nullptr))
        VK_VALIDATE(numFormats > 0, "No surface formats supported by device")

        gpu.surfaceFormats.SetCount((int)numFormats);
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.device, m_surface, &numFormats, gpu.surfaceFormats.Data()))
        VK_VALIDATE(numFormats > 0, "No surface formats supported by device")
    }

    {
        uint32_t numPresentModes;
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu.device, m_surface, &numPresentModes, nullptr))
        VK_VALIDATE(numPresentModes > 0, "No present modes supported by device")

        gpu.presentModes.SetCount((int)numPresentModes);
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu.device, m_surface, &numPresentModes, gpu.presentModes.Data()))
        VK_VALIDATE(numPresentModes > 0, "No present modes supported by device")
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
static bool CheckPhysicalDeviceExtensionSupport(GPUInfo & gpu, List<const char *> & requiredExt) {
    int required = requiredExt.Count();
    int available = 0;

    for (int i = 0; i < required; ++i) {
        int extensionPropsCount = gpu.extensionProps.Count();
        for (int j = 0; j < extensionPropsCount; ++j ) {
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
    m_vkContext.gpu = GPUInfo();
    m_vkContext.device = VK_NULL_HANDLE;
    m_vkContext.graphicsFamilyIdx = -1;
    m_vkContext.presentFamilyIdx = -1;
    m_vkContext.graphicsQueue = VK_NULL_HANDLE;
    m_vkContext.presentQueue = VK_NULL_HANDLE;
    m_vkContext.depthFormat = VK_FORMAT_UNDEFINED;
    m_vkContext.renderPass = VK_NULL_HANDLE;
    m_vkContext.pipelineCache = VK_NULL_HANDLE;
    m_vkContext.sampleCount = VK_SAMPLE_COUNT_1_BIT;
    m_vkContext.superSampling = false;
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
    m_instanceExtensions.Clear();
    m_deviceExtensions.Clear();
    m_validationLayers.Clear();

    m_surface = VK_NULL_HANDLE;
    m_presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

//    m_fullscreen = 0;
//    m_swapchain = VK_NULL_HANDLE;
//    m_swapchainFormat = VK_FORMAT_UNDEFINED;
//    m_currentSwapIndex = 0;
//    m_msaaImage = VK_NULL_HANDLE;
//    m_msaaImageView = VK_NULL_HANDLE;
//    m_commandPool = VK_NULL_HANDLE;
//
//    m_swapchainImages.Zero();
//    m_swapchainViews.Zero();
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
    CreateWindow();
    CreateInstance();
    CreateSurface();
    SelectPhysicalDevice();
    CreateLogicalDeviceAndQueues();
    CreateSemaphores();
    CreateQueryPool();
    CreateCommandPool();
    CreateCommandBuffer();
}

/*
================================================================================
RenderBackend::Shutdown

DESCRIPTION:
Shuts down the render backend.
================================================================================
*/
void RenderBackend::Shutdown() {
    vkFreeCommandBuffers(m_vkContext.device, m_commandPool, NUM_FRAME_DATA, m_commandBuffers);

    for (auto & m_commandBufferFence : m_commandBufferFences) {
        vkDestroyFence(m_vkContext.device, m_commandBufferFence, nullptr);
    }

    vkDestroyCommandPool(m_vkContext.device, m_commandPool, nullptr);

    for (auto & m_queryPool : m_queryPools) {
        vkDestroyQueryPool(m_vkContext.device, m_queryPool, nullptr );
    }

    for (int i = 0; i < (int)NUM_FRAME_DATA; ++i ) {
        vkDestroySemaphore(m_vkContext.device, m_acquireSemaphores[i], nullptr);
        vkDestroySemaphore(m_vkContext.device, m_renderCompleteSemaphores[i], nullptr);
    }

    if (Config::Get()->VulkanConfiguration().enableDebugLayer) {
        DestroyDebugReportCallback(m_instance);
    }

    vkDestroyDevice(m_vkContext.device, nullptr);
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
    WindowConfig windowConfig = Config::Get()->WindowConfiguration();
    uint32_t windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN;

    if (windowConfig.highDpi) {
        windowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
        SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
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
    VulkanConfig vkConfig = Config::Get()->VulkanConfiguration();

    uint32_t appVer = VK_MAKE_VERSION(vkConfig.programVersion.major,
                                      vkConfig.programVersion.minor,
                                      vkConfig.programVersion.patch);

    uint32_t engineVer = VK_MAKE_VERSION(vkConfig.engineVersion.major,
                                         vkConfig.engineVersion.minor,
                                         vkConfig.engineVersion.patch);

    uint32_t apiVer = VK_MAKE_VERSION(vkConfig.apiVersion.major,
                                      vkConfig.apiVersion.minor,
                                      vkConfig.apiVersion.patch);

    VkApplicationInfo appInfo   = {};
    appInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName    = vkConfig.programName;
    appInfo.applicationVersion  = appVer;
    appInfo.pEngineName         = vkConfig.engineName;
    appInfo.engineVersion       = engineVer;
    appInfo.apiVersion          = apiVer;

    VkInstanceCreateInfo createInfo     = {};
    createInfo.sType                    = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo         = &appInfo;

    m_instanceExtensions.Clear();
    m_deviceExtensions.Clear();
    m_validationLayers.Clear();

    uint32_t extensionCount;
    SDL_Vulkan_GetInstanceExtensions(m_window, &extensionCount, nullptr);
    m_instanceExtensions.SetCount((int)extensionCount);
    SDL_Vulkan_GetInstanceExtensions(m_window, &extensionCount, m_instanceExtensions.Data());

    for (auto & deviceExtension : g_deviceExtensions) {
        m_deviceExtensions.Add(deviceExtension);
    }

    if (vkConfig.enableValidationLayers) {
        for (auto & debugExtension : g_debugExtensions) {
            m_instanceExtensions.Add(debugExtension);
        }

        for (auto & validationLayer : g_validationLayers) {
            m_validationLayers.Add(validationLayer);
        }

        Vk_CheckValidationLayers();
    }

    createInfo.enabledExtensionCount    = (uint32_t)m_instanceExtensions.Count();
    createInfo.ppEnabledExtensionNames  = m_instanceExtensions.Data();
    createInfo.enabledLayerCount        = (uint32_t)m_validationLayers.Count();
    createInfo.ppEnabledLayerNames      = m_validationLayers.Data();

    VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_instance))

    if (vkConfig.enableDebugLayer) {
        Vk_CreateDebugReportCallback(m_instance);
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

    List<VkPhysicalDevice> devices;
    devices.SetCount((int)deviceCount);

    VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.Data()))
    VK_VALIDATE(deviceCount > 0, "Can't find Vulkan enabled device.")

    int bestScore = -1;
    GPUInfo bestGpu;

    for (int i = 0; i < (int)deviceCount; i++) {
        GPUInfo gpu = GetPhysicalDeviceInfo(devices[i]);

        if (gpu.score >= bestScore) {
            bestScore = gpu.score;
            bestGpu = gpu;
        }
    }

    int graphicsIdx = -1;
    int presentIdx = -1;

    if (!CheckPhysicalDeviceExtensionSupport(bestGpu, m_deviceExtensions)) {
        SDL_LogCritical(LOG_SYSTEM, "GPU doesn't support required extensions");
        exit(1);
    }

    if (bestGpu.surfaceFormats.Count() == 0) {
        SDL_LogCritical(LOG_SYSTEM, "GPU doesn't support surface formats.");
        exit(1);
    }

    if (bestGpu.presentModes.Count() == 0) {
        SDL_LogCritical(LOG_SYSTEM, "GPU doesn't support present modes.");
        exit(1);
    }

    // Find graphics queue family
    for (int j = 0; j < bestGpu.queueFamilyProps.Count(); ++j) {
        VkQueueFamilyProperties & props = bestGpu.queueFamilyProps[j];

        if (props.queueCount == 0) {
            continue;
        }

        if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsIdx = j;
            break;
        }
    }

    // Find present queue family
    for (int j = 0; j < bestGpu.queueFamilyProps.Count(); ++j) {
        VkQueueFamilyProperties & props = bestGpu.queueFamilyProps[j];

        if (props.queueCount == 0) {
            continue;
        }

        VkBool32 supportsPresent = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(bestGpu.device, (uint32_t)j, m_surface, &supportsPresent);
        if (supportsPresent) {
            presentIdx = j;
            break;
        }
    }

    if (graphicsIdx < 0 && presentIdx < 0) {
        // If we can't render or present, just bail.
        SDL_LogCritical(LOG_SYSTEM, "GPU doesn't meet the requirements");
        exit(1);
    }

    m_vkContext.graphicsFamilyIdx = graphicsIdx;
    m_vkContext.presentFamilyIdx = presentIdx;
    m_physicalDevice = bestGpu.device;
    m_vkContext.gpu = bestGpu;
}

/*
================================================================================
RenderBackend::CreateLogicalDeviceAndQueues

DESCRIPTION:
Selects the suitable graphics device.
================================================================================
*/
void RenderBackend::CreateLogicalDeviceAndQueues() {
    List<int> uniqueIdx;
    uniqueIdx.AddUnique(m_vkContext.graphicsFamilyIdx);
    uniqueIdx.AddUnique(m_vkContext.presentFamilyIdx);

    List<VkDeviceQueueCreateInfo> deviceQueueInfo;

    const float priority = 1.0f;
    const int uniqueIdxCount = uniqueIdx.Count();
    for (int i = 0; i < uniqueIdxCount; ++i ) {
        VkDeviceQueueCreateInfo queueInfo = {};
        queueInfo.sType             = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex  = (uint32_t)uniqueIdx[i];
        queueInfo.queueCount        = 1;
        queueInfo.pQueuePriorities  = &priority;

        deviceQueueInfo.Add(queueInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.textureCompressionBC     = VK_TRUE;
    deviceFeatures.imageCubeArray           = VK_TRUE;
    deviceFeatures.depthClamp               = VK_TRUE;
    deviceFeatures.depthBiasClamp           = VK_TRUE;
    deviceFeatures.depthBounds              = m_vkContext.gpu.features.depthBounds;
    deviceFeatures.fillModeNonSolid         = VK_TRUE;

    VkDeviceCreateInfo info = {};
    info.sType                      = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.queueCreateInfoCount       = (uint32_t)deviceQueueInfo.Count();
    info.pQueueCreateInfos          = deviceQueueInfo.Data();
    info.pEnabledFeatures           = &deviceFeatures;
    info.enabledExtensionCount      = (uint32_t)m_deviceExtensions.Count();
    info.ppEnabledExtensionNames    = m_deviceExtensions.Data();

    if (Config::Get()->VulkanConfiguration().enableValidationLayers) {
        info.enabledLayerCount = (uint32_t)m_validationLayers.Count();
        info.ppEnabledLayerNames = m_validationLayers.Data();
    } else {
        info.enabledLayerCount = 0;
    }

    VK_CHECK(vkCreateDevice(m_physicalDevice, &info, nullptr, &m_vkContext.device))

    vkGetDeviceQueue(m_vkContext.device, (uint32_t)m_vkContext.graphicsFamilyIdx, 0, &m_vkContext.graphicsQueue);
    vkGetDeviceQueue(m_vkContext.device, (uint32_t)m_vkContext.presentFamilyIdx, 0, &m_vkContext.presentQueue);
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
    semaphoreCreateInfo.sType           = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
//    semaphoreCreateInfo.pNext           = &timelineCreateInfo;
    semaphoreCreateInfo.flags           = 0;

    for (uint32_t i = 0; i < NUM_FRAME_DATA; ++i ) {
        VK_CHECK(vkCreateSemaphore(m_vkContext.device, &semaphoreCreateInfo, nullptr, &m_acquireSemaphores[i]));
        VK_CHECK(vkCreateSemaphore(m_vkContext.device, &semaphoreCreateInfo, nullptr, &m_renderCompleteSemaphores[i]));
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
        VK_CHECK(vkCreateQueryPool(m_vkContext.device, &createInfo, nullptr, &m_queryPool));
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
    createInfo.queueFamilyIndex  = (uint32_t)m_vkContext.graphicsFamilyIdx;

    VK_CHECK(vkCreateCommandPool(m_vkContext.device, &createInfo, nullptr, &m_commandPool));
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

    VK_CHECK(vkAllocateCommandBuffers(m_vkContext.device, &cbAllocateInfo, m_commandBuffers));

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    for (auto & m_commandBufferFence : m_commandBufferFences) {
        VK_CHECK(vkCreateFence(m_vkContext.device, &fenceCreateInfo, nullptr, &m_commandBufferFence));
    }
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