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
    architecture "x86_64"

    location "build"

    rootdir = os.getcwd()
    outputdir = "%{rootdir}/build/%{cfg.longname}"

    targetdir ("%{outputdir}")
    objdir    ("%{outputdir}/obj")

    include "dependencies"
    
    include "third_party/fmt/fmt_premake5"
    include "third_party/cJSON/cJSON_premake5"
    include "third_party/volk/volk_premake5"
    include "third_party/spdlog/spdlog_premake5"
    include "engine_premake5"
