#include <confini-1.h>
#include "Common.h"
#include "Game.h"

int main() {
    Game game;

    game.Init();
    game.Run();
    game.Shutdown();

    return 0;
}

