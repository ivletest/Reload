local p = premake

p.modules.copy_assets = {}
local copy_assets = p.modules.copy_assets

newaction {
    trigger     = "copy_assets",
    description = "Copies the game assets to the compiled game's directory.",
    execute 	= function ()

    end
}

return copy_assets