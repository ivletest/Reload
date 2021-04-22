project "spdlog"
    kind "StaticLib"
    language "C++"
    cppdialect "C++11"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/spdlog.cpp",
        "src/stdout_sinks.cpp",
        "src/color_sinks.cpp",
        "src/file_sinks.cpp",
        "src/async.cpp",
        "src/cfg.cpp",
        "include/spdlog/*h",
        "include/spdlog/cfg/*h",
        "include/spdlog/details/*h",
        "include/spdlog/sinks/*h",
        "include/spdlog/fmt/*h",
    }

    includedirs
    {
        "include",
        "../fmt/include",
    }
    links { "fmt" }
    defines { "SPDLOG_FMT_EXTERNAL", "SPDLOG_COMPILED_LIB" }

    filter "system:windows"
        buildoptions { "/Zc:__cplusplus" }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"