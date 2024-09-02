#pragma once
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
class Game
{
    private:
    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;

    public:
        Game();
        ~Game();
        void Initialize();
        void Run();
        void Destroy();
        void ProcessInput();
        void Update();
        void Render();
        void Setup();

        int windowWidth;
        int windowHeight;
};