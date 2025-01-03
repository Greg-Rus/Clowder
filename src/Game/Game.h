#pragma once
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"

const int FPS = 60;
const int MILLISECONDS_PER_FRAME = 1000 / FPS;

class Game
{
private:
    bool isRunning;
    bool isDebug;
    int millisecondsPreviousFrame = 0;
    SDL_Window *window;
    SDL_Renderer *renderer;

    std::unique_ptr<Registry> registry;
    std::unique_ptr<AssetStore> assetStore;

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
    void LoadLevel(int level);
    void LoadTileMap(const std::string& path);

    int windowWidth;
    int windowHeight;
};