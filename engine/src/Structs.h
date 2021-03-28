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
    Version         apiVersion;
    bool            enableValidationLayers;
    bool            enableDebugLayer;
    int             MSAALevels;
    const char *    programName;
    Version         programVersion;
    const char *    engineName;
    Version         engineVersion;
    int             selectedGpu;
    unsigned int    deviceLocalMemoryMB;
    unsigned int    uploadBufferSizeMB;
    int             swapInterval;
} VulkanConfig;

typedef struct Vertex {
    float position[2];
    float uv[2];
    uint8_t col[4];
} Vertex;

#ifdef DEBUG

typedef struct {
    float deltaTime;
} DebugLayerInfo;

#endif

#endif // !__SYS_STRUCTS_H