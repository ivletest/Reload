//
// Created by ivan on 30.3.21.
//

#include "Window.h"
#include "../Config.h"

SDL_Window * window;

/*
================================================================================
WindowManager::CreateWindow

DESCRIPTION:
Configures and creates the SDL Window.
================================================================================
*/
void WindowManager::CreateWindow() {
    uint32_t windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN;

    if (windowConfig.allowHighDpi) {
        windowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
        SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    }
    if (windowConfig.isFullScreen) {
        windowFlags |= SDL_WINDOW_FULLSCREEN;
    }

    window = SDL_CreateWindow(
            windowConfig.title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            static_cast<int>(windowConfig.width),
            static_cast<int>(windowConfig.height),
            windowFlags);

    if (!window) {
        SDL_LogCritical(LOG_VIDEO, "Failed to open window: %s\n", SDL_GetError());
        exit(1);
    }
}

/*
================================================================================
WindowManager::DestroyWindow

DESCRIPTION:
Destroys the SDL Window.
================================================================================
*/
void WindowManager::DestroyWindow() {
    SDL_DestroyWindow(window);
}