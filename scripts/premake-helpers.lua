--
-- Created by IntelliJ IDEA.
-- User: ivan
-- Date: 08.4.21
-- Time: 13:57
-- To change this template use File | Settings | File Templates.
--

function getFullPath(lib)
    return "".. os.getcwd() .. "/" .. lib .. "";
end

function getFullPathSubdir(lib)
    return "".. os.getcwd() .. "/" .. lib .. "/**";
end
