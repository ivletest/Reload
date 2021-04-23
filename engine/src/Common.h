#ifndef COMMON_H
#define COMMON_H

#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cstdbool>
#include <climits>
#include <algorithm>
#include <cassert>

#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <optional>

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_vulkan.h>
#include <volk.h>

#ifdef LINUX
#   include <X11/X.h>
#endif

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/printf.h>
#include <spdlog/spdlog.h>

#ifdef __unix__
#   define SEPARATOR   "/"
#   ifdef __linux__
#       include <linux/limits.h>
#       define MAX_PATH PATH_MAX
#   endif // __linux__
#else
#define SEPARATOR   "\\"
#endif // __unix__


#include "sys/Defs.h"
#include "Structs.h"
#include "sys/Paths.h"


// #ifdef RLD_DEBUG
//     #include "Renderer/debugLayer.h"
// #endif

#endif// !COMMON_H