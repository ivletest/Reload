#include "File.h"

#include <stdio.h>
#include <unistd.h>

#ifdef __linux__
 #include <linux/limits.h>
 #define RLD_PATH_MAX = PATH_MAX
#else
 #include <stdlib.h>
 #define RLD_PATH_MAX = _MAX_PATH
#endif

#include "../Common.h"

char * File::ReadAsString(const char *filepath){
    char cwd[MAX_PATH];

    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        SDL_LogError(LOG_FILE, "Error getting working directory.");
        exit(1);
    }

    size_t pathLen = strlen(cwd) + strlen(SEPARATOR) + strlen(filepath);

    if (pathLen > MAX_PATH) {
        SDL_LogError(LOG_FILE, "Path length is longer then what the OS supports.");
        exit(1);
    }

    char *fullFilePath = new char[pathLen];

    sprintf(fullFilePath, "%s%s%s", cwd, SEPARATOR, filepath);

    FILE *file;
    char *buffer;
    long  numbytes;

    file = fopen(fullFilePath, "rb");

    if (file == nullptr) {
        SDL_LogError(LOG_FILE, "Error opening %s.", fullFilePath);
        exit(1);
    }

    fseek(file, 0L, SEEK_END);
    numbytes = ftell(file);
    fseek(file, 0L, SEEK_SET);

    buffer = (char *)calloc((size_t)numbytes, sizeof(char));

    if (buffer == nullptr) {
        SDL_LogError(LOG_FILE, "Error creating buffer for %s.", fullFilePath);
        exit(1);
    }

    fread(buffer, sizeof(char), (size_t)numbytes, file);
    fclose(file);
    delete[] fullFilePath;

    return buffer;
}
