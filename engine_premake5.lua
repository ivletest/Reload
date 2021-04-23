project "ReloadEngineCore"
	kind 			"ConsoleApp" --"StaticLib"
	language 		"C++"
	cppdialect		"C++17"
	staticruntime 	"on"

-- package files
	files { "engine/src/**.h", "engine/src/**.hpp", "engine/src/**.cpp", "engine/src/**.inl" }

	includedirs {
		IncludeDir.Common,
		IncludeDir.ReloadEngine,
		IncludeDir.VulkanSDK,
		IncludeDir.Volk,
		IncludeDir.SDL2,
		IncludeDir.cJSON,
		IncludeDir.Fmt,
		IncludeDir.Spdlog
	}

	libdirs {
		LibraryDir.Common,
		LibraryDir.VulkanSDK,
		LibraryDir.SDL2
	}

	links {
		Library.Common,
		Library.Vulkan,
		Library.Volk,
		Library.SDL2,
		Library.cJSON,
		Library.Fmt,
		Library.Spdlog
	}

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


	-- WINDOWS
	filter "system:windows"
		defines { "WIN32", "_WINDOWS" }		

	-- LINUX
	filter "system:linux"
		defines { "LINUX", "_X11" }
		
	
	-- MACOSX
	filter "system:macosx"
		defines { "MACOSX" }

	-- linking libs
	filter "system:windows"
		-- To Add
	filter "system:linux"
		-- Whatever is here gets passed to ld like -lTheNameIPut
		linkoptions { "-lm -ldl -lpthread" }
		linkoptions{ "-Wl,-rpath=\\$$ORIGIN" }
		links { "X11", "pthread" }
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
			"{COPY} engine/assets %{cfg.targetdir}"
		}
	filter "system:macosx"
		postbuildcommands { }

	-- platform specific compiler options
 	-- mac lovin - they do this -framework thing, hence the no "OgreMain" under Mac's linking libs
	filter "system:windows"
		buildoptions { }
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
