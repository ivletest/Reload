project "volk"
kind "StaticLib"
language "C"
staticruntime "on"

targetdir ("build/bin/" .. outputdir .. "/%{prj.name}")
objdir ("build/obj/" .. outputdir .. "/%{prj.name}")

files  {  "volk.h", "volk.c" }

includedirs { "" }
links { "vulkan" }

defines( "VOLK_STATIC_DEFINES" )

filter "system:windows"
    defines { "VK_USE_PLATFORM_WIN32_KHR" }
filter "system:linux"
    includedirs("/usr/include/vulkan")

filter "configurations:Debug"
runtime "Debug"
symbols "on"

filter "configurations:Release"
runtime "Release"
optimize "on"