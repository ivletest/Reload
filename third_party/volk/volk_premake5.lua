project "volk"
kind "StaticLib"
language "C"
staticruntime "on"

files  {  "volk.h", "volk.c" }

includedirs { "", IncludeDir.VulkanSDK }
libdirs { LibraryDir.VulkanSDK }
links { Library.Vulkan }

defines( "VOLK_STATIC_DEFINES" )

filter "system:windows"
    defines { "VK_USE_PLATFORM_WIN32_KHR" }

filter "configurations:Debug"
runtime "Debug"
symbols "on"

filter "configurations:Release"
runtime "Release"
optimize "on"