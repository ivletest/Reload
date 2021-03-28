#ifndef __RELOAD_RENDER_BACKEND_H
#define __RELOAD_RENDER_BACKEND_H

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "renderer/vulkan/vk_mem_alloc.h"

#include "../Common.h"
#include "RenderCommon.h"

struct GPUInfo {
    VkPhysicalDevice                        device;
    VkPhysicalDeviceProperties              props;
    VkPhysicalDeviceMemoryProperties        memProps;
    VkPhysicalDeviceFeatures                features;
    VkSurfaceCapabilitiesKHR                surfaceCaps;
    std::vector<VkSurfaceFormatKHR>         surfaceFormats;
    std::vector<VkPresentModeKHR>           presentModes;
    std::vector<VkQueueFamilyProperties>    queueFamilyProps;
    std::vector<VkExtensionProperties>	    extensionProps;
    int                                     score;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    [[nodiscard]] bool AreSupported() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct VkContext {
    GPUInfo                 gpu;
    VkDevice                device;
    VkQueue					graphicsQueue;
    VkQueue					presentQueue;
    QueueFamilyIndices      queueFamilies;
    VkFormat				depthFormat;
    VkRenderPass			renderPass;
    VkPipelineCache			pipelineCache;
    VkSampleCountFlagBits   sampleCount;
    bool					superSampling;
};

extern VulkanConfig vkConfig;
extern WindowConfig windowConfig;
extern VkContext vkContext;

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
    void                        CreateLogicalDeviceAndQueues();                 // Creates logical device and queues.
    void                        CreateSemaphores();                             // Creates semaphores for image acquisition and rendering completion.
    void                        CreateQueryPool();                              // Creates Query Pool.
    void                        CreateCommandPool();                            // Creates Command Pool.
    void                        CreateCommandBuffer();                          // Creates Command Buffer.

    void                        CreateMemoryAllocator();                        // Creates the Vulkan memory allocator.
    void                        DestroyMemoryAllocator();                       // Destroys the Vulkan memory allocator.

    void				        CreateSwapChain();                              // Creates the swap chain.
    void				        DestroySwapChain();                             // Destroys the swap chain.

    void				        CreateRenderTargets();                          // Creates the render targets.
    void				        DestroyRenderTargets();                         // Destroys the render targets.

    void				        CreateRenderPass();                             // Creates a render pass.

    void				        CreateFrameBuffers();                           // Creates frame buffers.
    void				        DestroyFrameBuffers();                          // Destroys frame buffers.

    VkExtent2D                  ChooseSurfaceExtent(VkSurfaceCapabilitiesKHR & caps);

    void                        ClearContext();                                 // Clears the vulkan context and resets its  values.
    void                        Clear();                                        // Clears and resets all the values.

    SDL_Window *                m_window;
    VkInstance                  m_instance;
    VkPhysicalDevice            m_physicalDevice;
    VkSampleCountFlagBits       m_msaaSamples;
    VkDevice                    m_device;
    uint32_t                    m_queueIndex;
    VkQueue                     m_queue;
    VkDebugReportCallbackEXT    m_debugCallbackHandle;
    VkDebugUtilsMessengerEXT    m_debugUtilsCallbackHandle;
    VkSurfaceKHR                m_surface;
    VkSurfaceFormatKHR          m_surfaceFormat;
    VkSurfaceCapabilitiesKHR    m_surfaceCapabilities;
    VkPresentModeKHR            m_presentMode;
    int                         m_windowWidth;
    int                         m_windowHeight;
    VkExtent2D                  m_extent;

    std::vector<const char *>	m_instanceExtensions;
    std::vector<const char *>	m_deviceExtensions;
    std::vector<const char *>	m_validationLayers;

    // AMD Vulkan Memory Allocator
    VmaAllocator                m_allocator;
    VmaAllocation				m_msaaVmaAllocation;
    VmaAllocationInfo			m_msaaAllocation;

    VkCommandPool			    m_commandPool;

    int							m_fullscreen;
    VkSwapchainKHR				m_swapChain;
    VkFormat					m_swapChainFormat;
    VkExtent2D					m_swapChainExtent;
    uint32_t					m_currentSwapIndex;
    VkImage						m_msaaImage;
    VkImageView					m_msaaImageView;

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