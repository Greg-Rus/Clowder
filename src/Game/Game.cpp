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

Game::Game()
{
  isRunning = false;
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
  windowWidth = 800;  // displayMode.w /2;
  windowHeight = 600; // displayMode.h /2;

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

  assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
  assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
  assetStore->AddTexture(renderer, "jungle-map", "./assets/images/jungle.png");


  //TODO: Load tile map.

  Entity tank = registry->CreateEntity();

  tank.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0),glm::vec2(1.0, 1.0), 0.0);
  tank.AddComponent<RigidBodyComponent>(glm::vec2(50.0, 20.0));
  tank.AddComponent<SpriteComponent>("tank-image", 32, 32);

  Entity truck = registry->CreateEntity();

  truck.AddComponent<TransformComponent>(glm::vec2(50.0, 100.0),glm::vec2(1.0, 1.0), 0.0);
  truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 50.0));
  truck.AddComponent<SpriteComponent>("truck-image", 32, 32);
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
  registry->Update();
}

void Game::Render()
{
  SDL_SetRenderDrawColor(renderer, 10, 100, 50, 255);
  SDL_RenderClear(renderer);

  registry->GetSystem<RenderSystem>().Update(renderer, assetStore);

  SDL_RenderPresent(renderer);
}
