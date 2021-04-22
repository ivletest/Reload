workspace(settings.third_party_workspace)
    configurations { "Debug", "Release" }
	outputdir = "%{cfg.system}/%{cfg.longname}"

    include('common/fmt/fmt_premake5')
    include("common/cJSON/cJSON_premake5")
    include("common/volk/volk_premake5")
    include("common/VulkanMemoryAllocator/vma_premake5")
    include("common/spdlog/spdlog_premake5")