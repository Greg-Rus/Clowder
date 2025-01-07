#pragma once
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../EventBus/EventBus.h"

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
    SDL_Rect camera;

    std::unique_ptr<Registry> registry;
    std::unique_ptr<AssetStore> assetStore;
    std::unique_ptr<EventBus> eventBus;

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

    static int windowWidth;
    static int windowHeight;
    static int mapWidth;
    static int mapHeight;
};