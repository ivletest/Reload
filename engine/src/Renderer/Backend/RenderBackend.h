#ifndef RELOAD_RENDER_BACKEND_H
#define RELOAD_RENDER_BACKEND_H

#include "RenderCommon.h"
#include "VulkanCommon.h"
#include "ReloadLib/Containers/Array.h"

struct BackEndCounters {
    int		    c_surfaces = 0;
    int		    c_shaders = 0;

    int		    c_drawElements = 0;
    int		    c_drawIndexes = 0;

    int		    c_shadowElements = 0;
    int		    c_shadowIndexes = 0;

    int		    c_copyFrameBuffer = 0;

    float	    c_overDraw = 0;

    uint64_t    totalMicroSec = 0;		// total microseconds for backend run
    uint64_t	shadowMicroSec = 0;
    uint64_t	depthMicroSec = 0;
    uint64_t	interactionMicroSec = 0;
    uint64_t	shaderPassMicroSec = 0;
    uint64_t	gpuMicroSec = 0;
};

class RenderBackend {
public:
                RenderBackend();
                ~RenderBackend();

    void        Init();                                                         // Initializes the render backend.
    void        Shutdown();
    void        Restart();

    void        StartFrame();                                                   // Starts the frame drawing logic on the screen.
    void        EndFrame();                                                     // Ends the frame drawing logic and submits the result to the queue.
    void        SwapBuffers();                                                  // Swaps the front and back buffers.
    void        ClearView(
                    bool isColor,
                    bool isDepth,
                    bool isStencil,
                    uint32_t stencilValue,
                    float r,
                    float g,
                    float b,
                    float a);                                                   // Clears the view/screen with the specified color.

//    void		CopyFrameBuffer( idImage * image, int x, int y, int imageWidth, int imageHeight );
//
//    void		DepthBoundsTest( const float zmin, const float zmax );
//    void		PolygonOffset( float scale, float bias );
    void        Execute();                                                      // Executes the render commands.
    void        DrawView();                                                     // Draws the view onto the screen.
    void		Scissor(int x /* left*/, int y /* bottom */, int w, int h);     // Updates the Scissors/Clipping rectangle transformation parameters.
    void		Viewport(int x /* left */, int y /* bottom */, int w, int h);   // Updates the viewport transformation parameters.
//    inline void	Scissor( const idScreenRect & rect ) { GL_Scissor( rect.x1, rect.y1, rect.x2 - rect.x1 + 1, rect.y2 - rect.y1 + 1 ); }
//    inline void	Viewport( const idScreenRect & rect ) { GL_Viewport( rect.x1, rect.y1, rect.x2 - rect.x1 + 1, rect.y2 - rect.y1 + 1 ); }
//
//    void		Color( float r, float g, float b, float a );
//    inline void	Color( float r, float g, float b ) { GL_Color( r, g, b, 1.0f ); }
//    void		Color( float * color );


private:
    void		CreateInstance();                                               // Configures and creates the Vulkan instance.
    void        CreateSurface();                                                // Creates the presentation m_surface.

    GPU         GetDeviceInfo(VkPhysicalDevice device);                         // Gets the physical graphics device properties.
    void	    SelectBestGpu();                                                // Selects the best graphics device available on the system.
    void        CreateLogicalDeviceAndQueues();                                 // Creates device device and queues.

    void        CreateSwapchain();                                              // Creates the swap chain.
    void        DestroySwapchain();                                             // Destroys the swap chain.

    void        CreateFrameBuffers();                                           // Creates frame buffers.
    void        DestroyFrameBuffers();                                          // Destroys frame buffers.

    void        CreateSyncObjects();                                            // Create semaphores for image acquisition and rendering completion, and fences.
    void        DestroySyncObjects();                                           // Destroys the semaphores and fences.

    void        CreateQueryPool();                                              // Creates the query pool.
    void        DestroyQueryPool();                                             // Destroys the query pool.

    void        CreateCommandPool();                                            // Creates the command pool.
    void        CreateCommandBuffer();                                          // Creates Command Buffer.

    void		CreateRenderTargets();                                          // Creates the render targets.
    void		DestroyRenderTargets();                                         // Destroys the render targets.

    void		CreateRenderPass();                                             // Creates a render pass.

    void        ClearContext();                                                 // Clears the vulkan context and resets its  values.
    void        Clear();                                                        // Clears and resets all the values.

    BackEndCounters	            m_pc;

    uint64_t					m_counter;
    uint32_t					m_currentFrame;
    const ViewDefiniton *	    m_viewDef;

    VkSurfaceKHR					m_surface;
    VkPresentModeKHR				m_presentMode;
    VkCommandPool					m_commandPool;

    std::vector<const char *>	m_deviceExtensions;
    std::vector<const char *>	m_validationLayers;

    int	                m_fullscreen = 0;
    // Swapchain
    VkSwapchainKHR      m_swapchain;
    VkFormat            m_swapchainFormat;
    VkExtent2D          m_swapchainExtent;
    uint32_t            m_currentSwapIdx;
    VkImage             m_currentImage;
    VkImageView	        m_currentImageView;

    std::array<VkImage, MAX_FRAMES_IN_FLIGHT>           m_swapchainImages;
    std::array<VkImageView, MAX_FRAMES_IN_FLIGHT>       m_swapchainViews;
    std::array<VkFramebuffer, MAX_FRAMES_IN_FLIGHT>     m_frameBuffers;

    std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT>   m_commandBuffers;
    std::array<VkFence, MAX_FRAMES_IN_FLIGHT>			m_commandBufferFences;
    std::array<bool, MAX_FRAMES_IN_FLIGHT>        	    m_commandBufferRecorded;

    std::array<uint32_t, MAX_FRAMES_IN_FLIGHT>          m_queryIndex;
    std::array<std::array<uint64_t, NUM_TIMESTAMP_QUERIES>, MAX_FRAMES_IN_FLIGHT> m_queryResults;
    std::array<VkQueryPool, MAX_FRAMES_IN_FLIGHT>       m_queryPools;
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT>       m_imgAvailableSemaphores;
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT>       m_renderCompleteSemaphores;
    std::array<VkFence, MAX_FRAMES_IN_FLIGHT>           m_inFlightFences;

    std::array<VkFence, MAX_FRAMES_IN_FLIGHT>                m_imagesInFlight;
};

#endif // RELOAD_RENDER_BACKEND_H