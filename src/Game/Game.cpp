#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <ostream>
#include <glm/glm.hpp>
#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/CoreComponents.h"
#include "../Systems/CoreSystems.h"
#include <iostream>
#include <fstream>
#include <sstream>

Game::Game()
{
  isRunning = false;
  isDebug = false;
  registry = std::make_unique<Registry>();
  assetStore = std::make_unique<AssetStore>();

  Logger::Log("Game Constructor Called!");
}

Game::~Game()
{
  Logger::Log("Game Destructor Called!");
}

void Game::Initialize()
{
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
  {
    Logger::Error("Failed to initialize SDL");
    return;
  };
  SDL_DisplayMode displayMode;
  SDL_GetCurrentDisplayMode(0, &displayMode);
  windowWidth = displayMode.w / 2;
  windowHeight = displayMode.h / 2;

  window = SDL_CreateWindow(
      "Clowder Game Engine",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      windowWidth,
      windowHeight,
      0);

  if (!window)
  {
    Logger::Error("Failed to create SDL window");
    return;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer)
  {
    Logger::Error("Failed to create SDL renderer");
    return;
  }

  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
  isRunning = true;
  Logger::Log("Game Initialized!");
}

void Game::LoadLevel(int level)
{
  registry->AddSystem<MovementSystem>();
  registry->AddSystem<RenderSystem>();
  registry->AddSystem<AnimationSystem>();
  registry->AddSystem<CollisionSystem>();
  registry->AddSystem<DebugRenderColliderSystem>();

  assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
  assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
  assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper.png");
  assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");

  assetStore->AddTexture(renderer, "jungle-map", "./assets/tilemaps/jungle.png");

  LoadTileMap("./assets/tilemaps/jungle.map");

  Entity chopper = registry->CreateEntity();
  chopper.AddComponent<TransformComponent>(glm::vec2(10.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
  chopper.AddComponent<RigidBodyComponent>(glm::vec2(50.0, 20.0));
  chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 2);
  chopper.AddComponent<AnimationComponent>(2, 10, true);
  chopper.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));

  Entity radar = registry->CreateEntity();
  radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 64 - 10, 10), glm::vec2(1.0, 1.0), 0.0);
  radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2);
  radar.AddComponent<AnimationComponent>(8, 10, true);

  Entity tank = registry->CreateEntity();
  tank.AddComponent<TransformComponent>(glm::vec2(200.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
  tank.AddComponent<RigidBodyComponent>(glm::vec2(-30, 0.0));
  tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
  tank.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));

  Entity truck = registry->CreateEntity();
  truck.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
  truck.AddComponent<RigidBodyComponent>(glm::vec2(30.0, 0.0));
  truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
  truck.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));
}

void Game::LoadTileMap(const std::string &tileMapPath)
{
  std::ifstream file(tileMapPath);
  std::string line;
  std::vector<std::vector<int> *> map;

  while (std::getline(file, line))
  {
    std::vector<int> *newRow = new std::vector<int>();
    std::string token;
    std::istringstream tokenStream(line);
    while (std::getline(tokenStream, token, ','))
    {
      int number = std::stoi(token);
      newRow->push_back(number);
    }
    map.push_back(newRow);
  }

  file.close();
  int tileSize = 32;
  double tileScale = 2.0;
  for (auto row = 0; row < map.size(); row++)
  {
    for (auto col = 0; col < map[row]->size(); col++)
    {
      int tileId = map[row]->at(col);
      int mapRow = tileId * 0.1;
      int mapCol = tileId - (mapRow * 10);
      int tileX = col * tileSize;
      int tileY = row * tileSize;
      int srcRectX = mapCol * tileSize;
      int srcRectY = mapRow * tileSize;
      Logger::Log("Id: " + std::to_string(tileId) +
                  "X: " + std::to_string(srcRectX) + " Y: " + std::to_string(srcRectY));

      Entity tile = registry->CreateEntity();
      tile.AddComponent<TransformComponent>(glm::vec2(tileX * tileScale, tileY * tileScale), glm::vec2(tileScale, tileScale), 0.0);
      tile.AddComponent<SpriteComponent>("jungle-map", 32, 32, 0, srcRectX, srcRectY);
    }
  }
}

void Game::Setup()
{

  LoadLevel(1);
  Logger::Log("Setup");
}

void Game::Run()
{
  Setup();
  while (isRunning)
  {
    ProcessInput();
    Update();
    Render();
  }
}

void Game::Destroy()
{
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  Logger::Log("Game Destroy!");
}

void Game::ProcessInput()
{
  SDL_Event sdlEvent;
  while (SDL_PollEvent(&sdlEvent))
  {
    switch (sdlEvent.type)
    {
    case SDL_QUIT:
      isRunning = false;
      break;
    case SDL_KEYDOWN:
      if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
      {
        isRunning = false;
      }
      if (sdlEvent.key.keysym.sym == SDLK_d)
      {
        isDebug = !isDebug;
      }
      break;
    }
  }
}

void Game::Update()
{
  int timeToWait = MILLISECONDS_PER_FRAME - (SDL_GetTicks() - millisecondsPreviousFrame);
  if (timeToWait > 0 && timeToWait <= MILLISECONDS_PER_FRAME)
  {
    SDL_Delay(timeToWait);
  }

  double deltaTime = (SDL_GetTicks() - millisecondsPreviousFrame) / 1000.0;

  millisecondsPreviousFrame = SDL_GetTicks();

  registry->GetSystem<MovementSystem>().Update(deltaTime);
  registry->GetSystem<AnimationSystem>().Update();
  registry->GetSystem<CollisionSystem>().Update();
  registry->Update();
}

void Game::Render()
{
  SDL_SetRenderDrawColor(renderer, 10, 100, 50, 255);
  SDL_RenderClear(renderer);

  registry->GetSystem<RenderSystem>().Update(renderer, assetStore);
  if (isDebug)
  {
    registry->GetSystem<DebugRenderColliderSystem>().Update(renderer);
  }

  SDL_RenderPresent(renderer);
}
