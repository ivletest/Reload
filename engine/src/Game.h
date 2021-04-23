#ifndef __GAME_H
#define __GAME_H

#include "precompiled.h"

#include "Common.h"
#include "Renderer/RenderSystem.h"

class Game {
public:
                    Game();
                    ~Game();

    void            Init();                                                     // Initializes all systems for the game.
    void            Run();                                                      // Runs the game loop until quit is requested.
    void            Shutdown();                                                 // Shuts down all the systems.

private:
    static void     InitSDL();                                                  // Initializes the SDL2 library.
    void            ProcessInput();
    void            Update();
    void            Render();

    RenderSystem    m_renderSystem;

    bool            m_isRunning = false;
};

#endif // !__GAME_H
