require 'defines'
require 'settings'
require 'helpers'
require 'triggers'
require 'common'
require 'cmake'
require 'clion'

workspace(settings.reload_engine)
    configurations { "Debug", "Release" }
    flags { 'MultiProcessorCompile' }
    language 		"C++"
    cppdialect		"C++17"
    configurations { "Debug", "Release" }

    location "build"
    outputdir = "%{rootdir}/build/%{os.outputof('uname')}/%{cfg.longname}"

    targetdir ("%{outputdir}")
    objdir    ("%{outputdir}/obj")

    include "engine/engine_premake5"
    include "third_party/libs_premake5"
