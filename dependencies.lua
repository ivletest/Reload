VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir  = {}
LibraryDir  = {}
Library     = {}

if os.host() == "windows" then
    -- Include directories
    IncludeDir.Global = { }
    IncludeDir.VulkanSDK = "%{VULKAN_SDK}/Include"
    IncludeDir.SDL2 = "third_party/windows/SDL2/include"
    IncludeDir.OpenAL = ""
    
    -- Library directories
    LibraryDir.GLobal = { }
    LibraryDir.VulkanSDK = "%{VULKAN_SDK}/Lib"
    LibraryDir.SDL2 = "third_party/windows/SDL2/lib"

    -- Libraries
    Library.Vulkan = "vulkan-1"
    Library.OpenAL = "OpenAL32"
end

if os.host() == "linux" then
    -- Includes
    IncludeDir.Global = { "/usr/include", "/usr/local/include" }
    IncludeDir.VulkanSDK = "/usr/include/vulkan"
    IncludeDir.SDL2 = "/usr/include/SDL2"
    IncludeDir.OpenAL = "third_party/common/OpenAL/include"

    -- Library directories
    LibraryDir.Global = { "/usr/lib/", "/usr/local/lib/" }
    LibraryDir.VulkanSDK = ""
    LibraryDir.SDL2 = ""

    -- Libraries
    Library.Vulkan = "vulkan"
    Library.OpenAL = "OpenAL"
end
-- Includes
IncludeDir.ReloadEngine = "engine/src"
IncludeDir.Volk = "third_party/common/volk"
IncludeDir.cJSON = "third_party/common/cJSON"
IncludeDir.Fmt = "third_party/common/fmt/include"
IncludeDir.Spdlog = "third_party/common/spdlog/include"

-- Libraries
Library.Volk = "volk"
Library.cJSON = "cJSON"
Library.Fmt = "fmt"
Library.Spdlog = "spdlog"
