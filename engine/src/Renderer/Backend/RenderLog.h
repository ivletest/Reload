//
// Created by ivan on 01.4.21.
//

#ifndef RELOAD_RENDERER_LOGGER_H
#define RELOAD_RENDERER_LOGGER_H

#include "precompiled.h"
#include "../Common.h"
#include "VulkanCommon.h"
#include "RenderCommon.h"

void Log_PhysicalDeviceInfo(GPU gpu);
void Log_GpuWarn(std::string msg);
void Log_GpuCritical(std::string msg);
void Log_GpuError(std::string msg);

#endif //RELOAD_RENDERER_LOGGER_H
