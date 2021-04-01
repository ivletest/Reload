#include "Game.h"

#include "renderer/RenderBackend.h"
#include "Config.h"

static const uint32_t MS_PER_UPDATE = 16;

/*
================================================================================
Game::Game

DESCRIPTION:
The default game class constructor.
================================================================================
*/
Game::Game() = default;

/*
================================================================================
Game::~Game

DESCRIPTION:
The default game class destructor.
================================================================================
*/
Game::~Game() = default;

/*
================================================================================
Game::Init

DESCRIPTION:
Initializes all systems for the game.
================================================================================
*/
void Game::Init() {

    InitSDL();
    m_renderSystem.Init();
}

/*
================================================================================
Game::Shutdown

DESCRIPTION:
Shuts down all the systems.
================================================================================
*/
void Game::Shutdown() {
    m_renderSystem.Shutdown();
}

/*
================================================================================
Game::InitSDL

DESCRIPTION:
Initializes the SDL2 library.
================================================================================
*/
void Game::InitSDL() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_LogCritical(LOG_SYSTEM, "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
}

/*
================================================================================
Class::Method

DESCRIPTION:
RETURNS:
NOTE:
================================================================================
*/
void Game::ProcessInput() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                m_isRunning = false;
                break;

            default:
                break;
        }
    }
}

/*
================================================================================
Class::Method

DESCRIPTION:
RETURNS:
NOTE:
================================================================================
*/
void Game::Update() {
}

/*
================================================================================
Class::Method

DESCRIPTION:
RETURNS:
NOTE:
================================================================================
*/
void Game::Render() {
    m_renderSystem.PrepareScene();

    m_renderSystem.PresentScene();
}
/*
================================================================================
Game::Run

DESCRIPTION:
Runs the game loop until quit is requested.
================================================================================
*/
void Game::Run() {
    uint32_t previous = SDL_GetTicks();
    uint32_t lag = 0;

    m_isRunning = true;

    while (m_isRunning) {
        uint32_t current = SDL_GetTicks();
        uint32_t deltaTime = current - previous;

        previous = current;
        lag += deltaTime;

        ProcessInput();

        while (lag >= MS_PER_UPDATE) {
            Update();
            lag -= MS_PER_UPDATE;
        }

        Render();
    }
}