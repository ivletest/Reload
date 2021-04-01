//
// Created by ivan on 20.3.21.
//

#include "Config.h"

#include <cjson/cJSON.h>
#include "lib/File.h"

#ifndef DEBUG
    #define MAIN_CONFIG "sys_config.json"
#else
    #define MAIN_CONFIG "sys_config.debug.json"
#endif

GameInfo        gameInfo;
WindowConfig    windowConfig;
VulkanConfig    vkConfig;

/*
================================================================================
Config::SysLoad

DESCRIPTION:
Loads the default configuration from the `sys_config.json` located in the src
directory.
================================================================================
*/
void ConfigManager::LoadSystemConfig() {
    char *buffer = File::ReadAsString(MAIN_CONFIG);
    cJSON *config = cJSON_Parse(buffer);

    free(buffer);

    if (config == nullptr) {
        const char *errorPtr = cJSON_GetErrorPtr();
        if (errorPtr != nullptr) {
            fprintf(stderr, "Error before: %s\n", errorPtr);
            goto free_mem_and_return;
        }
    }

    //
    // Get game configuration
    //
    {
        const cJSON *gameInfoJson = cJSON_GetObjectItem(config, "game");
        const cJSON *name = cJSON_GetObjectItem(gameInfoJson, "name");
        const cJSON *version = cJSON_GetObjectItem(gameInfoJson, "version");
        const cJSON *versionMajor = cJSON_GetObjectItem(version, "major");
        const cJSON *versionMinor = cJSON_GetObjectItem(version, "minor");
        const cJSON *versionPatch = cJSON_GetObjectItem(version, "patch");

        if (!cJSON_IsString(name)) {
            printf("Game configuration error. Name field is not a string");
            goto free_mem_and_return;
        }
        if (!cJSON_IsNumber(versionMajor)) {
            printf("Game configuration error. Version major field is not number");
            goto free_mem_and_return;
        }
        if (!cJSON_IsNumber(versionMinor)) {
            printf("Game configuration error. Version minor field is not number");
            goto free_mem_and_return;
        }
        if (!cJSON_IsNumber(versionPatch)) {
            printf("Game configuration error. Version patch field is not number");
            goto free_mem_and_return;
        }

        gameInfo.name = name->valuestring;
        gameInfo.version.major = (unsigned int) versionMajor->valueint;
        gameInfo.version.minor = (unsigned int) versionMinor->valueint;
        gameInfo.version.patch = (unsigned int) versionPatch->valueint;
    }

    //
    // Get window configuration
    //
    {
        const cJSON *windowConfigJson = cJSON_GetObjectItem(config, "window");
        const cJSON *title = cJSON_GetObjectItem(windowConfigJson, "title");
        const cJSON *width = cJSON_GetObjectItem(windowConfigJson, "width");
        const cJSON *height = cJSON_GetObjectItem(windowConfigJson, "height");
        const cJSON *highDpi = cJSON_GetObjectItem(windowConfigJson, "allowHighDpi");
        const cJSON *fullScreen = cJSON_GetObjectItem(windowConfigJson, "isFullScreen");

        if (!cJSON_IsString(title)) {
            printf("Window configuration error. Title field is not a string");
            goto free_mem_and_return;
        }
        if (!cJSON_IsNumber(width)) {
            printf("Window configuration error. Width field is not number");
            goto free_mem_and_return;
        }
        if (!cJSON_IsNumber(height)) {
            printf("Window configuration error. Height field is not number");
            goto free_mem_and_return;
        }
        if (!cJSON_IsBool(highDpi)) {
            printf("Window configuration error. High DPI field is not boolean");
            goto free_mem_and_return;
        }
        if (!cJSON_IsBool(fullScreen)) {
            printf("Window configuration error. Full screen field is not boolean");
            goto free_mem_and_return;
        }

        windowConfig.title = title->valuestring;
        windowConfig.width = (unsigned int) width->valueint;
        windowConfig.height = (unsigned int) height->valueint;
        windowConfig.allowHighDpi = (bool) cJSON_IsTrue(highDpi);
        windowConfig.isFullScreen = (bool) cJSON_IsTrue(fullScreen);
    }
    //
    // Get vulkan configuration
    //
    {
        const cJSON *vkConfigJson = cJSON_GetObjectItem(config, "vulkan");
        const cJSON *version = cJSON_GetObjectItem(vkConfigJson, "version");
        const cJSON *versionMajor = cJSON_GetObjectItem(version, "major");
        const cJSON *versionMinor = cJSON_GetObjectItem(version, "minor");
        const cJSON *versionPatch = cJSON_GetObjectItem(version, "patch");
        const cJSON *enableValidationLayers = cJSON_GetObjectItem(vkConfigJson, "enableValidationLayers");
        const cJSON *enableDebugLayer = cJSON_GetObjectItem(vkConfigJson, "enableDebugLayer");
        const cJSON *MSAALevels = cJSON_GetObjectItem(vkConfigJson, "MSAALevels");
        const cJSON *selectedGpu = cJSON_GetObjectItem(vkConfigJson, "selectedGpu");
        const cJSON *deviceLocalMemoryMB = cJSON_GetObjectItem(vkConfigJson, "deviceLocalMemoryMB");
        const cJSON *uploadBufferSizeMB = cJSON_GetObjectItem(vkConfigJson, "uploadBufferSizeMB");
        const cJSON *swapInterval = cJSON_GetObjectItem(vkConfigJson, "swapInterval");

        if (!cJSON_IsNumber(versionMajor)) {
            printf("Vulkan configuration error. Version major field is not number");
            goto free_mem_and_return;
        }
        if (!cJSON_IsNumber(versionMinor)) {
            printf("Vulkan configuration error. Version minor field is not number");
            goto free_mem_and_return;
        }
        if (!cJSON_IsNumber(versionPatch)) {
            printf("Vulkan configuration error. Version patch field is not number");
            goto free_mem_and_return;
        }
        if (!cJSON_IsBool(enableValidationLayers)) {
            printf("Vulkan configuration error. Enable Validation Layers field is not boolean");
            goto free_mem_and_return;
        }
        if (!cJSON_IsBool(enableDebugLayer)) {
            printf("Vulkan configuration error. Enable Debug Layers field is not boolean");
            goto free_mem_and_return;
        }
        if (!cJSON_IsNumber(MSAALevels)) {
            printf("Vulkan configuration error. MSAA Levels field is not number");
            goto free_mem_and_return;
        }
        if (!cJSON_IsNumber(selectedGpu)) {
            printf("Vulkan configuration error. Selected GPU field is not number");
            goto free_mem_and_return;
        }
        if (!cJSON_IsNumber(deviceLocalMemoryMB)) {
            printf("Vulkan configuration error. Device local memory field is not number");
            goto free_mem_and_return;
        }
        if (!cJSON_IsNumber(uploadBufferSizeMB)) {
            printf("Vulkan configuration error. upload buffer size field is not number");
            goto free_mem_and_return;
        }

        if (!cJSON_IsNumber(swapInterval)) {
            printf("Vulkan configuration error. swap interval field is not number");
            goto free_mem_and_return;
        }

        vkConfig.apiVersion.major = (unsigned int) versionMajor->valueint;
        vkConfig.apiVersion.minor = (unsigned int) versionMinor->valueint;
        vkConfig.apiVersion.patch = (unsigned int) versionPatch->valueint;
        vkConfig.enableValidationLayers = (bool) cJSON_IsTrue(enableValidationLayers);
        vkConfig.enableDebugLayer = (bool) cJSON_IsTrue(enableDebugLayer);
        vkConfig.MSAALevels = MSAALevels->valueint;
        vkConfig.programName = gameInfo.name;
        vkConfig.programVersion = gameInfo.version;
        vkConfig.engineName = gameInfo.name;
        vkConfig.engineVersion = gameInfo.version;
        vkConfig.selectedGpu =  selectedGpu->valueint;
        vkConfig.deviceLocalMemoryMB = (unsigned int)deviceLocalMemoryMB->valueint;
        vkConfig.uploadBufferSizeMB = (unsigned int)uploadBufferSizeMB->valueint;
        vkConfig.swapInterval = swapInterval->valueint;
    }

    free_mem_and_return:
    cJSON_Delete(config);
}