//
// Created by ivan on 19.3.21.
//

#include "RenderSystem.h"

RenderSystem::RenderSystem() {
    m_Initialized = false;
}

RenderSystem::~RenderSystem() {
}

/*
================================================================================
RenderSystem::Init

DESCRIPTION:
Initializes the rendering system.
================================================================================
*/
void RenderSystem::Init() {
    if(m_Initialized) {
        SDL_LogWarn(LOG_SYSTEM, "RenderSystem already initialized.");
        return;
    }

    SDL_LogInfo(LOG_SYSTEM, "Initializing render system\n");

    m_backend.Init();
}

/*
================================================================================
RenderSystem::Shutdown

DESCRIPTION:
Shuts down the rendering system.
================================================================================
*/
void RenderSystem::Shutdown() {
    m_backend.Shutdown();
}

void RenderSystem::PrepareScene() {
    m_backend.PrepareScene();
}

void RenderSystem::PresentScene() {
    m_backend.PresentScene();
}