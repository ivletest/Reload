require 'scripts/premake-defines'
require 'scripts/premake-common'
require 'scripts/premake-triggers'
require 'scripts/premake-settings'
require 'scripts/premake-vscode/vscode'
require 'scripts/premake-cmake/cmake'

language 		"C++"
cppdialect		"C++17"

root_dir = os.getcwd()

Arch = ""

if _OPTIONS["arch"] then
	Arch = _OPTIONS["arch"]
else
	if _OPTIONS["os"] then
		_OPTIONS["arch"] = "arm"
		Arch = "arm"
	else
		_OPTIONS["arch"] = "x64"
		Arch = "x64"
	end
end

--------------------------------------------------------------------------------
-- WORKSPACE CONFIGURATIONS
--------------------------------------------------------------------------------

workspace(settings.workspace_name)
    configurations { "Debug", "Release"}
    flags { 'MultiProcessorCompile' }
    
	configurations { "Debug", "Release"}
    
    location "build"
	outputdir = "%{cfg.system}/%{cfg.longname}"

	targetdir ("build/bin/%{outputdir}/")
	objdir    ("build/obj/%{outputdir}/")       

    if Arch == "arm" then 
		architecture "ARM"
	elseif Arch == "x64" then 
		architecture "x86_64"
	elseif Arch == "x86" then
		architecture "x86"
    end
    
    print("Arch = ", Arch)

    configurations { "Debug", "Release" }

	include "engine/lib/cJSON/premake5"
    include "engine/premake5"
 