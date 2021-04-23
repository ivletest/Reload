#include "precompiled.h"

#include "ConfigManager.h"
#include "Game.h"

int main(int argc, char* argv[]) {
    ConfigManager::LoadSystemConfig();
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

    Game game;

    game.Init();
    game.Run();
    game.Shutdown();

    return 0;
}

