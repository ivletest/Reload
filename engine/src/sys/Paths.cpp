#include "Paths.h"

#include <string.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL_filesystem.h>

Paths paths;

void paths_Init(void)
{
//    int ret = 0;

    // Set root directories
    paths.program_root = SDL_GetBasePath();

//    ret = sprintf(paths.assets_root,  "%sassets", paths.program_root);
//    if (ret < 0) exit(1);
//    ret = sprintf(paths.scripts_root, "%sscripts", paths.program_root);
//    if (ret < 0) exit(1);
//    ret = sprintf(paths.config_root,  "%sconfig", paths.program_root);
//    if (ret < 0) exit(1);


    // Set sub-directories
//    ret = sprintf(paths.assets_shaders, "%s%s%s", paths.assets_root, SEPARATOR, "shaders");
//    if (ret < 0) exit(1);
//    ret = sprintf(paths.assets_fonts,   "%s%s%s", paths.assets_root, SEPARATOR, "fonts"  );
//    if (ret < 0) exit(1);
//    ret = sprintf(paths.assets_models,  "%s%s%s", paths.assets_root, SEPARATOR, "models" );
//    if (ret < 0) exit(1);
}

char *paths_Join(const char *path, const char *file)
{
    char *result = (char *)malloc(MAX_PATH);
    int ret = sprintf(result, "%s%s%s", path, SEPARATOR, file);
    if (ret < 0) exit(1);

    return result;
}
