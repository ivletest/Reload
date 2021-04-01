#ifndef __SYS_STRUCTS_H
#define __SYS_STRUCTS_H

//==================================================================================================
// Configuration structures
//==================================================================================================

typedef struct Version {
    unsigned int    major;
    unsigned int    minor;
    unsigned int    patch;
} Version;

typedef struct GameInfo {
    const char *    name;
    Version         version;
} GameConfig;

typedef struct WindowConfig {
    const char *    title;
    unsigned int    width;
    unsigned int    height;
    bool            allowHighDpi;
    bool            isFullScreen;
} WindowConfig;

typedef struct VulkanConfig {
    int             MSAALevels;
    int             selectedGpu;
    int             swapInterval;
    unsigned int    deviceLocalMemoryMB;
    unsigned int    uploadBufferSizeMB;
    Version         apiVersion;
    Version         programVersion;
    Version         engineVersion;
    bool            enableValidationLayers;
    bool            enableDebugLayer;
    const char *    programName;
    const char *    engineName;
} VulkanConfig;

#endif // !__SYS_STRUCTS_H