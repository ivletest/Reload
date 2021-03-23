--------------------------------------------------------------------------------
-- PROJECT CONFIGURATION
--------------------------------------------------------------------------------

IncludeDir = {}
IncludeDir["openal"] = "../lib/openal/include"
IncludeDir["VulkanLinux"] = "/usr/include/vulkan"
IncludeDir["SDL2Linux"] = "/usr/include/SDL2"
IncludeDir["cJSON"] = "../lib/cJSON"
IncludeDir["LinuxLocalInclude"] = "/usr/local/include"

project "ReloadEngineCore"
	kind 			"ConsoleApp" --"StaticLib"
	language 		"C++"
	cppdialect		"C++17"
	staticruntime 	"on"
	pchheader "src/precompiled.h"
	pchsource "src/precompiled.cpp"

	-- package files
	files { "src/**.h", "src/**.cpp", "src/**.inl" }

	includedirs { "src/" }

	sysincludedirs {
		"%{IncludeDir.openal}",
		"%{IncludeDir.LinuxLocalInclude}",
		"%{IncludeDir.cJSON}"
	}

	links {
		"vulkan",
		"cJSON"
	}

	-- defines
	filter "system:windows"
		defines { "WIN32", "_WIN32", "_WINDOWS" }
	filter "system:linux"
		defines { "LINUX", "_X11" }
	filter "system:macosx"
		defines { "MACOSX" }

	filter "architecture:x86_64"
		defines { "RLD_SSE" ,"USE_VMA_ALLOCATOR" }

	--configuration
	filter { "configurations:Debug" }
		defines { "DEBUG", "_DEBUG" }
		symbols "On"
		runtime "Debug"
		optimize "Debug"

	filter { "configurations:Release" }
		defines { "NDEBUG", "_NDEBUG" }
		optimize "Speed"
		symbols "On"
		runtime "Release"

	-- include paths
	filter "system:windows"		
		sysincludedirs {
		}
	filter "system:linux"
		sysincludedirs {
			"%{IncludeDir.VulkanLinux}",
			"%{IncludeDir.SDL2Linux}"
		}
	filter "system:macosx"
		sysincludedirs {
		}

	-- library paths
	filter "system:windows"
		libdirs {
			"../vendor/openal/libs/Win32"
		}
	filter "system:linux"
		libdirs { 
			"../vendor",
			"/usr/lib",
			"/usr/local/lib" 
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
			"-lm"
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
			"{COPY} ../reload/assets %{cfg.targetdir}",
			"{COPY} ../reload/core/usr_config.json %{cfg.targetdir}",
		}
		filter { "configurations:Debug" }
			postbuildcommands {
				"{COPY} ../reload/core/sys_config.debug.json %{cfg.targetdir}",
			}
		filter { "configurations:Release" }
			postbuildcommands {
				"{COPY} ../reload/core/sys_config.json %{cfg.targetdir}",
			}
	filter "system:macosx"
		postbuildcommands { }

	-- platform specific compiler options
 	-- mac lovin - they do this -framework thing, hence the no "OgreMain" under Mac's linking libs
	filter "system:windows"
		buildoptions {  }
	filter "system:linux"
		buildoptions { 
			"-fPIC",
			"-Wall",
			"-Wextra",
			"-Wconversion",
			"-pedantic",
			 "-std=gnu++17"
		}
		filter {'system:linux', 'architecture:x86_64'}
			buildoptions {"-msse4.1" }
	filter "system:macosx"
		buildoptions { "-framework OpenAL" }

	filter "files:**.ttf"

	-- this is simply a message shown in the Visual Studio output
	buildmessage "converting %{file.relpath} to dds ..."

	-- this is the actual custom compilation command
	buildcommands {
		"ddsconverter --input=%{file.abspath} --output=%{cfg.linktarget.directory}"
	}
