//
// Created by ivan on 04.4.21.
//

#include "RenderPipelineManager.h"

/*
================================================================================
RenderPipelineManager::RenderPipelineManager

DESCRIPTION:
The default constructor.
================================================================================
*/
RenderPipelineManager::RenderPipelineManager()
    : m_counter(0)
    , m_currentData(0){}


/*
================================================================================
RenderPipelineManager::Init

DESCRIPTION:
Initializes the rendering pipeline.
================================================================================
*/
void RenderPipelineManager::Init() {

}

/*
================================================================================
RenderPipelineManager::Shutdown

DESCRIPTION:
Shuts down the rendering pipeline.
================================================================================
*/
void RenderPipelineManager::Shutdown() {

}

/*
================================================================================
RenderPipelineManager::StartFrame

DESCRIPTION:
Starts the frame drawing logic on the screen.
================================================================================
*/
void RenderPipelineManager::StartFrame() {

}

/*
================================================================================
RenderPipelineManager::Submit

DESCRIPTION:
Commits the command buffer to execute.
================================================================================
*/
void RenderPipelineManager::Submit(uint64_t stateBits, VkCommandBuffer cmdBuffer) {

}

/*
================================================================================
RenderPipelineManager::GetPipeline

DESCRIPTION:
Gets the rendering pipeline.
RETURNS:
The rendering pipeline.
================================================================================
*/
VkPipeline RenderPipelineManager::GetPipeline() {
    return nullptr;
}
