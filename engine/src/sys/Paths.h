#ifndef _SYS_PATHS_JOIN_H
#define _SYS_PATHS_JOIN_H

// #define USE_SRT
#include "../Common.h"

//==============================================================================
// Structure definitions
//==============================================================================

#ifndef USE_SRT
typedef struct Paths
{
    const char *program_root;
    const char *assets_root;
    const char *scripts_root;
    const char *config_root;
    const char *assets_shaders;
    const char *assets_fonts;
    const char *assets_models;
} Paths;

#else
typedef struct pahts_t
{
    srt_string *program_root;
    srt_string *assets_root;
    srt_string *scripts_root;
    srt_string *config_root;
    srt_string *assets_shaders;
    srt_string *assets_fonts;
    srt_string *assets_models;
} Paths;
#endif
extern Paths paths;

void paths_Init(void);
// srt_string *paths_Join(srt_string *path, const char *file);
char *paths_Join(const char *path, const char *file);

#endif // !_SYS_PATHS_JOIN_H