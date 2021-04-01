//
// Created by ivan on 30.3.21.
//

#ifndef RELOAD_WINDOW_H
#define RELOAD_WINDOW_H

#include "../Common.h"

extern SDL_Window * window;

namespace WindowManager {
    void CreateWindow();
    void DestroyWindow();
}

#endif //RELOAD_WINDOW_H
