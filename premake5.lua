require 'defines'
require 'settings'
require 'helpers'
require 'triggers'
require 'common'
require 'cmake'
require 'clion'

workspace(settings.engine_workspace)
    configurations { "Debug", "Release"}
    flags { 'MultiProcessorCompile' }
    language 		"C++"
    cppdialect		"C++17"
    configurations { "Debug", "Release"}

    location "build"
    outputdir = "%{cfg.system}/%{cfg.longname}"

    include "third_party/libs_premake5"
    include "engine/engine_premake5"