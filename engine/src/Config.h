//
// Created by ivan on 20.3.21.
//

#ifndef RELOAD_CONFIG_H
#define RELOAD_CONFIG_H

#include "Common.h"

extern GameInfo                    gameInfo;
extern WindowConfig                windowConfig;
extern VulkanConfig                vkConfig;

namespace ConfigManager {
    void LoadSystemConfig();
}

#endif //RELOAD_C_CONFIG_H
