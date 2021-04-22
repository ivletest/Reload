project "cJSON"
    kind "StaticLib"
    language "C"
    staticruntime "on"

    files
    {
        "cJSON.h",
        "cJSON.c",
        "cJSON_Utils.h",
        "cJSON_Utils.c",
    }

    includedirs { "" }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"



