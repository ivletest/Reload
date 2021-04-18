//
// Created by ivan on 08.4.21.
//
#include "RenderLog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

static auto console = spdlog::stdout_color_mt("renderer_console");
static auto err_console = spdlog::stderr_color_mt("renderer_err_console");
/*
================================================================================
RenderLog::PhysicalDeviceInfo

DESCRIPTION:
Logs graphics device information on the console output.
================================================================================
*/
void Log_PhysicalDeviceInfo(GPU gpu) {
    std::string gpuType;
    std::string gpuVendor;
    std::string apiVersion;
    std::string extensions;

    switch (static_cast<int32_t>(gpu.props.deviceType)) {
        case 1:
            gpuType = "Integrated";
            break;
        case 2:
            gpuType = "Discrete";
            break;
        case 3:
            gpuType = "Virtual";
            break;
        case 4:
            gpuType = "CPU";
            break;
        default:
            gpuType = fmt::format("Other {}", gpu.props.deviceType);
    }

    switch (gpu.props.vendorID) {
        case 0x8086:
            gpuVendor = "\"Intel\"";
            break;
        case 0x10DE:
            gpuVendor = "\"Nvidia\"";
            break;
        case 0x1002:
            gpuVendor = "\"AMD\"";
            break;
        default:
            gpuVendor = fmt::format("\"{}\"", gpu.props.vendorID);
    }

    uint32_t ver[3] = {
            VK_VERSION_MAJOR(gpu.props.apiVersion),
            VK_VERSION_MINOR(gpu.props.apiVersion),
            VK_VERSION_PATCH(gpu.props.apiVersion)
    };

    apiVersion = fmt::format("API Version: {0}.{1}.{2}", ver[0], ver[1], ver[2]);

#if 0
    extensions = "Extensions: \n";
    for (const auto &extension : gpu.extensionProps) {
        extensions.append(fmt::format("\t{}\n", extension.extensionName));
    }
#endif
    console->info(
            "{0} Physical Device: {1} {2} {3}\n{4}\n{5}\n",
            gpuType,
            gpu.props.deviceID,
            gpuVendor,
            fmt::format("\"{}\"", gpu.props.deviceName),
            apiVersion,
            extensions);
}

void Log_GpuWarn(std::string msg) {
    console->warn("GPU WARNING: {}", msg);
}

void Log_GpuCritical(std::string msg) {
    console->critical("GPU CRITICAL ERROR: {}", msg);
}

void Log_GpuError(std::string msg) {
    console->error("GPU ERROR: {}", msg);
}

