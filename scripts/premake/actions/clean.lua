premake.modules.clean = {}
local clear = premake.modules.clean

newaction
    {
        trigger     = "clean",
        description = "clean the software",
        execute     = function ()
            print("clean the build...")
            os.rmdir("./build")
            print("done.")
        end
    }

return clean