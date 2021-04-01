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
    VkDevice                logicalDevice;
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

    void                        ClearViewport(
                                    bool isColor,
                                    bool isDepth,
                                    bool isStencil,
                                    uint32_t stencilValue,
                                    float r,
                                    float g,
                                    float b,
                                    float a);                                   // Clears the viewport/screen with the specified color.


private:
    void				        CreateInstance();                               // Configures and creates the Vulkan instance.
    void                        CreateSurface();                                // Configures and creates The Vulkan surface to be drawn to.
    GPUInfo                     GetPhysicalDeviceInfo(VkPhysicalDevice device); // Gets the physical graphics device information.
    void				        SelectPhysicalDevice();                         // Selects the suitable graphics device.
    void                        CreateLogicalDeviceAndQueues();                 // Creates logical device and queues.

    void                        CreateSemaphores();                             // Creates semaphores for image acquisition and rendering completion.
    void                        DestroySemaphores();                            //Destroys semaphores for image acquisition and rendering completion.

    void                        CreateQueryPool();                              // Creates the query pool.
    void                        DestroyQueryPool();                             // Destroys the query pool.

    void                        CreateCommandPool();                            // Creates the command pool.
    void                        DestroyCommandBuffer();                         // Destroys the command pool.

    void                        CreateCommandBuffer();                          // Creates Command Buffer.

    void                        CreateMemoryAllocator();                        // Creates the Vulkan memory allocator.

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

    uint64_t					m_counter;
    uint32_t					m_currentFrameData;

    VkInstance                  m_instance;
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

    std::array<VkImage, NUM_FRAME_DATA>			m_swapChainImages;
    std::array<VkImageView, NUM_FRAME_DATA>		m_swapChainViews;
    std::array<VkFramebuffer, NUM_FRAME_DATA>   m_frameBuffers;

    std::array<VkCommandBuffer, NUM_FRAME_DATA> m_commandBuffers;
    std::array<VkFence, NUM_FRAME_DATA>         m_commandBufferFences;
    std::array<bool, NUM_FRAME_DATA>        	m_commandBufferRecorded;
    std::array<VkSemaphore, NUM_FRAME_DATA>     m_acquireSemaphores;
    std::array<VkSemaphore, NUM_FRAME_DATA>     m_renderCompleteSemaphores;

    std::array<uint32_t, NUM_FRAME_DATA>        m_queryIndex;
    std::array<uint64_t, NUM_TIMESTAMP_QUERIES> m_queryResults[NUM_FRAME_DATA];
    std::array<VkQueryPool, NUM_FRAME_DATA>     m_queryPools;
};

#endif // __RELOAD_RENDER_BACKEND_H