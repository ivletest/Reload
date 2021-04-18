#ifndef SYS_DEFS_H
#define SYS_DEFS_H

#include "precompiled.h"

#define SCREEN_WIDTH   1280
#define SCREEN_HEIGHT  720

#define xstr(x) str(x)

#define TIME_T int64_t

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

#endif // !SYS_DEFS_H