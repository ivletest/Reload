//
// Created by ivan on 04.4.21.
//

#ifndef RELOAD_RENDER_PIPELINE_MANAGER_H
#define RELOAD_RENDER_PIPELINE_MANAGER_H

#include "precompiled.h"

#include "Common.h"
#include "VulkanCommon.h"

struct RenderProg {
    uint64_t    stateBits;
    VkPipeline  pipeline;
};

class RenderPipelineManager {
public:
    RenderPipelineManager();
    ~RenderPipelineManager() = default;

    void Init();                                                                // Initializes the rendering pipeline.
    void Shutdown();                                                            // Shuts down the rendering pipeline.
    void StartFrame();                                                          // Starts the frame drawing logic on the screen.
    void Submit(uint64_t stateBits, VkCommandBuffer cmdBuffer);                 // Commits the command buffer to execute.
    VkPipeline GetPipeline();                                                   // Gets the rendering pipeline.

private:
    int					    m_counter;
    int					    m_currentData;

    std::vector<RenderProg> m_renderProgs;

};


#endif // !RELOAD_RENDER_PIPELINE_MANAGER_H
