require 'defines'
require 'settings'
require 'helpers'
require 'triggers'
require 'common'
require 'cmake'
require 'clion'

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

workspace(settings.reload_engine)
    configurations { "Debug", "Release" }
    flags { 'MultiProcessorCompile' }
    language 		"C++"
    cppdialect		"C++17"
    configurations { "Debug", "Release" }

    if Arch == "arm" then
        architecture "ARM"
    elseif Arch == "x64" then
        architecture "x86_64"
    elseif Arch == "x86" then
        architecture "x86"
    end

    location "build"
    outputdir = "%{rootdir}/build/%{os.host()}/%{cfg.longname}"
    targetdir ("%{outputdir}")
    objdir    ("%{outputdir}/obj")

    include "engine/engine_premake5"
    include "third_party/libs_premake5"
