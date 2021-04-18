//
// Created by ivan on 30.3.21.
//

#include "Window.h"
#include "SDL_syswm.h"
#include "../ConfigManager.h"

WindowManager windowManager;
RWindow window;

static List<RDisplay> displays;

bool EnumerateDisplayModes(int vidMode, int & width, int & height, int & display) {
    width = 1;
    height = 1;
    display = 0;

    if (vidMode < displays.Size()) {
        return false;
    }

    int displayModesSize = displays[display].modes.Size();
    while (vidMode >= displayModesSize) {
        vidMode -= displayModesSize;
        display++;
        if (display >= displays.Size()) {
            return false;
        }
    }

    uint32_t mode = displays[display].modes[vidMode];
    width = mode >> 16;
    height = mode & 0xFFFF;

    return true;
}

/*
================================================================================
WindowManager::CreateWindow

DESCRIPTION:
Configures and creates the SDL Window.
================================================================================
*/
void WindowManager::CreateWindow() {

    int width, height, display;
    uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN;

    if (windowConfig.allowHighDpi) {
        flags |= SDL_WINDOW_ALLOW_HIGHDPI;
        SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    }

    if (windowConfig.isFullScreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
//        if (!EnumerateDisplayModes(r_mode.GetInteger(), width, height, display)) {
            EnumerateDisplayModes(0, width, height, display);
//        }
    } else {
        width = windowConfig.width;
        height = windowConfig.height;
        display = 0;
    }

    window.handle = SDL_CreateWindow(
            windowConfig.title,
            static_cast<int>(SDL_WINDOWPOS_CENTERED_DISPLAY(display)),
            static_cast<int>(SDL_WINDOWPOS_CENTERED_DISPLAY(display)),
            width,
            height,
            flags);

    if (!window.handle) {
        window.handle = SDL_CreateWindow(
                windowConfig.title,
                static_cast<int>(SDL_WINDOWPOS_CENTERED_DISPLAY(display)),
                static_cast<int>(SDL_WINDOWPOS_CENTERED_DISPLAY(display)),
                windowConfig.width,
                windowConfig.height,
                flags);
    }

    if (!window.handle) {
        SDL_LogCritical(LOG_VIDEO, "Failed to open window: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_GetWindowSize(window.handle, &window.width, &window.height);

    window.pixelAspect = 1.0f;

//    SDL_SysWMinfo wm;
//
//    SDL_VERSION(&wm.version);
//    SDL_GetWindowWMInfo(window.handle, &wm);
//
}

/*
================================================================================
WindowManager::DestroyWindow

DESCRIPTION:
Destroys the SDL Window.
================================================================================
*/
void WindowManager::DestroyWindow() {
    SDL_DestroyWindow(window.handle);
}

void WindowManager::EnumerateDisplays() {
    displays.Clear();

    int numDisplays = SDL_GetNumVideoDisplays();

    for (int i = 0; i < numDisplays; ++i ) {
        RDisplay display;

        SDL_DisplayMode currentMode;
        if (SDL_GetCurrentDisplayMode( i, &currentMode ) == 0 ) {
            display.width = currentMode.w;
            display.height = currentMode.h;
            display.height = currentMode.refresh_rate;
        } else {
            spdlog::warn("Could not get video mode for display %d\n", i);
            display.width = static_cast<int>(windowConfig.width);
            display.height = static_cast<int>(windowConfig.height);
        }

        display.isDefault = ( i == 0 );

        int numModes = SDL_GetNumDisplayModes( i );
        for ( int j = 0; j < numModes; ++j ) {
            SDL_DisplayMode mode;
            if (SDL_GetDisplayMode(i, j, &mode) != 0) {
                continue;
            }

            if (SDL_BITSPERPIXEL(mode.format) != 24 ) {
                continue;
            }

            if (mode.refresh_rate != 60 && mode.refresh_rate != 120) {
                continue;
            }

            if (mode.h < 720) {
                continue;
            }

            display.modes.AddUnique(static_cast<uint32_t>((mode.w << 16) | (mode.h & 0xFFFF)));
        }

        displays.Add(display);
    }
}