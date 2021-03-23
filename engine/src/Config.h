//
// Created by ivan on 20.3.21.
//

#ifndef RELOAD_CONFIG_H
#define RELOAD_CONFIG_H

#include "Common.h"

class Config {
public:
    Config(Config &other) = delete;
    void operator=(const Config &) = delete;

    static Config *             Get();
    void                        LoadSystemConfig();
    [[nodiscard]] GameInfo      GameInformation() const { return m_gameInfo; };
    [[nodiscard]] WindowConfig  WindowConfiguration() const { return m_windowConfig; };
    [[nodiscard]] VulkanConfig  VulkanConfiguration() const { return m_vkConfig; };
private:
                                Config() = default;
                                ~Config() = default;
    static Config *             s_instance;

    GameInfo                    m_gameInfo;
    WindowConfig                m_windowConfig;
    VulkanConfig                m_vkConfig;
};

#endif //RELOAD_C_CONFIG_H
