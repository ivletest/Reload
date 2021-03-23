#ifndef __SYS_DEFS_H
#define __SYS_DEFS_H

#include <SDL2/SDL.h>

#define SCREEN_WIDTH   1280
#define SCREEN_HEIGHT  720

#define str(x) #x
#define xstr(x) str(x)

enum {
    LOG_SYSTEM      = SDL_LOG_CATEGORY_SYSTEM,
    LOG_ASSERT      = SDL_LOG_CATEGORY_ASSERT,
    LOG_AUDIO       = SDL_LOG_CATEGORY_AUDIO,
    LOG_VIDEO       = SDL_LOG_CATEGORY_VIDEO,
    LOG_RENDER      = SDL_LOG_CATEGORY_RENDER,
    LOG_INPUT       = SDL_LOG_CATEGORY_INPUT,
    LOG_FILE        = SDL_LOG_CATEGORY_CUSTOM,

};

#define VERSION(major, minor, patch) \
    ((((uint32_t)(major)) << 22) | (((uint32_t)(minor)) << 12) | ((uint32_t)(patch)))

#define ENGINE_NAME "Reload Engine"
#define ENGINE_VERSION VERSION(0, 1, 0)

#endif // !__DEFS_H