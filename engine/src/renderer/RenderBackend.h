#ifndef __RELOAD_RENDER_BACKEND_H
#define __RELOAD_RENDER_BACKEND_H

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "../Common.h"
#include "RenderCommon.h"
#include "../lib/containers/List.h"

struct GPUInfo {
    VkPhysicalDevice                    device;
    VkPhysicalDeviceProperties          props;
    VkPhysicalDeviceMemoryProperties    memProps;
    VkPhysicalDeviceFeatures            features;
    VkSurfaceCapabilitiesKHR            surfaceCaps;
    List<VkSurfaceFormatKHR>            surfaceFormats;
    List<VkPresentModeKHR>              presentModes;
    List<VkQueueFamilyProperties>       queueFamilyProps;
    List<VkExtensionProperties>		    extensionProps;
    int                                 score;
};

struct VkContext {
    GPUInfo                 gpu;
    VkDevice                device;
    int						graphicsFamilyIdx;
    int						presentFamilyIdx;
    VkQueue					graphicsQueue;
    VkQueue					presentQueue;

    VkFormat				depthFormat;
    VkRenderPass			renderPass;
    VkPipelineCache			pipelineCache;
    VkSampleCountFlagBits   sampleCount;
    bool					superSampling;
};

class RenderBackend {
public:
                                RenderBackend();
                                ~RenderBackend();

    void                        Init();                                         // Initializes the render backend.
    void                        Shutdown();
    void                        Restart();

    void                        PrepareScene();
    void                        PresentScene();

private:
    void                        CreateWindow();                                 // Configures and creates the SDL window.
    void				        CreateInstance();                               // Configures and creates the Vulkan instance.
    void                        CreateSurface();                                // Configures and creates The Vulkan surface to be drawn to.
    GPUInfo                     GetPhysicalDeviceInfo(VkPhysicalDevice device); // Gets the physical graphics device information.
    void				        SelectPhysicalDevice();                         // Selects the suitable graphics device.
    void                        CreateLogicalDeviceAndQueues();                 // Create logical device and queues.
    void                        CreateSemaphores();                             // Create semaphores for image acquisition and rendering completion.
    void                        CreateQueryPool();                              // Create Query Pool.
    void                        CreateCommandPool();                            // Create Command Pool.
    void                        CreateCommandBuffer();                          // Create Command Buffer.

    void                        ClearContext();                                 // Clears the vulkan context and resets its  values.
    void                        Clear();                                        // Clears and resets all the values.

    VkContext                   m_vkContext;

    SDL_Window *                m_window;
    VkInstance                  m_instance;
    VkPhysicalDevice            m_physicalDevice;
    VkSampleCountFlagBits       m_msaaSamples;
    VkDevice                    m_device;
    uint32_t                    m_queueIndex;
    VkQueue                     m_queue;
    VmaAllocator                m_allocator;
    VkDebugReportCallbackEXT    m_debugCallbackHandle;
    VkDebugUtilsMessengerEXT    m_debugUtilsCallbackHandle;
    VkSurfaceKHR                m_surface;
    VkSurfaceFormatKHR          m_surfaceFormat;
    VkSurfaceCapabilitiesKHR    m_surfaceCapabilities;
    VkPresentModeKHR            m_presentMode;
    int                         m_windowWidth;
    int                         m_windowHeight;
    VkExtent2D                  m_extent;

    List<const char *>			m_instanceExtensions;
    List<const char *>			m_deviceExtensions;
    List<const char *>			m_validationLayers;

    VmaAllocation				m_msaaVmaAllocation;
    VmaAllocationInfo			m_msaaAllocation;

    VkCommandPool			    m_commandPool;

    VkImage			            m_swapChainImages[NUM_FRAME_DATA];
    VkImageView		            m_swapChainViews[NUM_FRAME_DATA];
    VkFramebuffer           	m_frameBuffers[NUM_FRAME_DATA];

    VkCommandBuffer         	m_commandBuffers[NUM_FRAME_DATA];
    VkFence         			m_commandBufferFences[NUM_FRAME_DATA];
    bool        				m_commandBufferRecorded[NUM_FRAME_DATA];
    VkSemaphore         		m_acquireSemaphores[NUM_FRAME_DATA];
    VkSemaphore     	    	m_renderCompleteSemaphores[NUM_FRAME_DATA];

    uint32_t                    m_queryIndex[NUM_FRAME_DATA];
    uint64_t        	        m_queryResults[NUM_FRAME_DATA][NUM_TIMESTAMP_QUERIES];
    VkQueryPool         		m_queryPools[NUM_FRAME_DATA];
};

#endif // __RELOAD_RENDER_BACKEND_H