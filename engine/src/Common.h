#ifndef __COMMON_H
#define __COMMON_H

#include <stdio.h>
#include <string.h>
// #include <math.h>
// #include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <algorithm>
#include <string>


#ifdef __unix__
    #define SEPARATOR   "/"
    #ifdef __linux__
        #include <linux/limits.h>
        #define MAX_PATH PATH_MAX
    #endif // __linux__
#else
    #define SEPARATOR   "\\"
#endif // __unix__

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_log.h>
#include <vulkan/vulkan.h>

#include "sys/Defs.h"
#include "Structs.h"
#include "sys/Paths.h"


// #ifdef RLD_DEBUG
//     #include "renderer/debugLayer.h"
// #endif

#endif// !__COMMON_H