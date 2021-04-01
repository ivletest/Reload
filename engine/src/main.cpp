#include "Config.h"
#include "Game.h"

int main() {
    ConfigManager::LoadSystemConfig();

    Game game;

    game.Init();
    game.Run();
    game.Shutdown();

    return 0;
}

