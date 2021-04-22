project "vma"
    kind "StaticLib"
    language "C++"
    cppdialect "C++11"
    staticruntime "on"

    targetdir ("build/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("build/obj/" .. outputdir .. "/%{prj.name}")

    files  { "include/vk_mem_alloc.h", "src/vk_mem_alloc.cpp" }
    includedirs { "include" }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"