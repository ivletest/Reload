--------------------------------------------------------------------------------
-- PROJECT CONFIGURATION
--------------------------------------------------------------------------------

include "lib/libs_premake5"

project "ReloadEngineCore"
	kind 			"ConsoleApp" --"StaticLib"
	language 		"C++"
	cppdialect		"C++17"
	staticruntime 	"on"
	pchheader "src/precompiled.h"
	pchsource "src/precompiled.cpp"

	-- package files
	files { "src/**.h", "src/**.hpp", "src/**.cpp", "src/**.inl" }

	includedirs {
		"src/",
		"lib/volk",
		"lib/VulkanMemoryAllocator/include",
		"lib/cJSON",
		"lib/fmt/include",
		"lib/openal/include",
		"lib/spdlog/include"
	}

	links {
		"vulkan",
		"volk",
		"vma",
		"cJSON",
		"fmt",
		"spdlog"
	}

	filter "system:windows"
		defines { "WIN32", "_WINDOWS" }
	filter "system:linux"
		defines { "LINUX", "_X11" }
	filter "system:macosx"
		defines { "MACOSX" }

	filter "architecture:x86_64"
		defines { "RLD_SSE" ,"USE_VMA_ALLOCATOR" }

	--configuration
	filter "configurations:Debug"
		defines { "DEBUG", "RLD_DEBUG" }
		symbols "On"
		runtime "Debug"
		optimize "Debug"

	filter "configurations:Release"
		defines { "NDEBUG", "RLD_NDEBUG" }
		optimize "Speed"
		symbols "On"
		runtime "Release"

	-- include paths
	filter "system:windows"		
		sysincludedirs {
		}
	filter "system:linux"
		includedirs {
			"/usr/include",
			"/usr/local/include",
			"/usr/include/vulkan",
			"/usr/include/SDL2",
		}
	filter "system:macosx"
		sysincludedirs {
		}

	-- library paths
	filter "system:windows"
		libdirs {
			getFullPath("openal/libs/Win32")
		}
	filter "system:linux"
		libdirs { 
			"../vendor",
			"/usr/lib/",
			"/usr/local/lib/"
		}
	filter "system:macosx"
		-- See the bottom of the file for the mac
		-- way of doing this.
		libdirs {
		}

	-- linking libs
	filter "system:windows"
		links {
			"OpenAL32"
		}
	filter "system:linux"
		-- Whatever is here gets passed to ld like -lTheNameIPut
		linkoptions {
			"-lSDL2 -lSDL2_image -lSDL2_ttf",
			"-lm -ldl -lpthread"
		}
		linkoptions{ "-Wl,-rpath=\\$$ORIGIN" }
		links {
			"X11", 
			"pthread"
		}
	filter "system:macosx"
		-- Again, see the bottom of the file for the
		-- mac way of doing this.
		links {
		}

	-- post build commands

	filter "system:windows"
		postbuildcommands { }
	filter "system:linux"
		postbuildcommands {
			"{COPY} ../engine/assets %{cfg.targetdir}",
			"{COPY} ../engine/usr_config.json %{cfg.targetdir}",
		}
		filter { "configurations:Debug" }
			postbuildcommands {
				"{COPY} ../engine/sys_config.debug.json %{cfg.targetdir}",
			}
		filter { "configurations:Release" }
			postbuildcommands {
				"{COPY} ../engine/sys_config.json %{cfg.targetdir}",
			}
	filter "system:macosx"
		postbuildcommands { }

	-- platform specific compiler options
 	-- mac lovin - they do this -framework thing, hence the no "OgreMain" under Mac's linking libs
	filter "system:windows"
		buildoptions {
			"-fno-rtti"
		}
	filter "system:linux"
		buildoptions { 
			"-fPIC",
			"-Wall",
			"-Wextra",
			"-Wconversion",
			"-pedantic",
			"-std=gnu++17",
			"-fno-rtti"
		}
		filter {'system:linux', 'architecture:x86_64'}
			buildoptions {"-msse4.1" }
	filter "system:macosx"
		buildoptions {
			"-framework OpenAL",
			"-fno-rtti"
		}

	filter "files:**.ttf"

	-- this is simply a message shown in the Visual Studio output
	buildmessage "converting %{file.relpath} to dds ..."

	-- this is the actual custom compilation command
	buildcommands {
		"ddsconverter --input=%{file.abspath} --output=%{cfg.linktarget.directory}"
	}