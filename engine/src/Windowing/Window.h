//
// Created by ivan on 30.3.21.
//

#ifndef RELOAD_WINDOW_H
#define RELOAD_WINDOW_H

#include "precompiled.h"

#include "ReloadLib/Containers/List.h"

struct RDisplay {
    bool	isDefault = false;
    int		width = 0;
    int		height = 0;
    int		hz = 0;
    List<uint32_t> modes;
};

struct RWindow {
    SDL_Window *    handle;
    int             width;
    int             height;
    bool            isFullScreen;
    float           pixelAspect;
};

class WindowManager {
public:
    void EnumerateDisplays();
    void CreateSDLWindow();
    void DestroyWindow();
};

extern WindowManager windowManager;
extern RWindow window;

#endif //RELOAD_WINDOW_H
