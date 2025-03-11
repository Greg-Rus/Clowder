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
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>
#include "../Events/KeyPressedEvent.h"
#include "../Events/KeyUpEvent.h"
#include "LevelLoader.h"

int Game::windowWidth;
int Game::windowHeight;
int Game::mapWidth;
int Game::mapHeight;
// int Game::logicalWidth = 800;
// int Game::logicalHeight = 600;

Game::Game()
{
  isRunning = false;
  isDebug = false;
  registry = std::make_unique<Registry>();
  assetStore = std::make_unique<AssetStore>();
  eventBus = std::make_unique<EventBus>();

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

  if(TTF_Init() != 0)
  {
        Logger::Error("Failed to initialize SDL_TTF");
    return;
  }


  SDL_DisplayMode displayMode;
  SDL_GetCurrentDisplayMode(0, &displayMode);
  windowWidth = displayMode.w / 2;
  windowHeight = displayMode.h / 2;
  // windowWidth = 16 * 200;
  // windowHeight = 9 * 200;

  // windowWidth = logicalWidth * 2;
  // windowHeight = logicalHeight * 2;
  // windowWidth = logicalWidth;
  // windowHeight = logicalHeight;

  window = SDL_CreateWindow(
      "Clowder Game Engine",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      windowWidth,
      windowHeight,
      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

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
  //SDL_RenderSetScale(renderer, 2, 2);
  //SDL_RenderSetLogicalSize(renderer, logicalWidth, logicalHeight);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer2_Init(renderer);

  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

  camera.x = 0;
  camera.y = 0;
  camera.w = windowWidth;
  camera.h = windowHeight;

  isRunning = true;
  Logger::Log("Game Initialized!");
}

void Game::Setup()
{
  registry->AddSystem<MovementSystem>();
  registry->AddSystem<RenderSystem>();
  registry->AddSystem<AnimationSystem>();
  registry->AddSystem<CollisionSystem>();
  registry->AddSystem<DebugRenderColliderSystem>();
  registry->AddSystem<DamageSystem>();
  registry->AddSystem<KeyboardControlSystem>();
  registry->AddSystem<CameraMovementSystem>();
  registry->AddSystem<ProjectileEmitSystem>(registry);
  registry->AddSystem<ProjectileLifecycleSystem>();
  registry->AddSystem<RenderTextSystem>(assetStore);
  registry->AddSystem<RenderHealthSystem>(assetStore);
  registry->AddSystem<RenderGUISystem>(registry);

  LevelLoader loader;
  loader.LoadLevel(1, registry, assetStore, renderer);
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

void Game::ProcessInput()
{
  SDL_Event sdlEvent;
  while (SDL_PollEvent(&sdlEvent))
  {

    ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
    ImGuiIO& io = ImGui::GetIO();

    int mouse_x, mouse_y;
    io.MousePos = ImVec2(mouse_x, mouse_y);

    switch (sdlEvent.type)
    {
    case SDL_QUIT:
      isRunning = false;
      break;
    case SDL_KEYDOWN:
      eventBus->EmitEvent<KeyPressedEvent>(sdlEvent.key.keysym.sym);
      if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
      {
        isRunning = false;
      }
      if (sdlEvent.key.keysym.sym == SDLK_HOME)
      {
        isDebug = !isDebug;
      }
      break;

    case SDL_KEYUP:
      eventBus->EmitEvent<KeyUpEvent>(sdlEvent.key.keysym.sym);
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

  eventBus->Reset();

  registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
  registry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(eventBus);
  registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);
  registry->GetSystem<MovementSystem>().SubscribeToEvents(eventBus);

  registry->Update();

  registry->GetSystem<MovementSystem>().Update(deltaTime);
  registry->GetSystem<AnimationSystem>().Update();
  registry->GetSystem<CollisionSystem>().Update(eventBus);
  registry->GetSystem<CameraMovementSystem>().Update(camera);
  registry->GetSystem<ProjectileEmitSystem>().Update();
  registry->GetSystem<ProjectileLifecycleSystem>().Update();
  registry->GetSystem<KeyboardControlSystem>().Update();
}

void Game::Render()
{
  SDL_SetRenderDrawColor(renderer, 10, 100, 50, 255);
  SDL_RenderClear(renderer);
  // ImGui_ImplSDLRenderer2_NewFrame();
  // ImGui_ImplSDL2_NewFrame();
  // ImGui::NewFrame();

  registry->GetSystem<RenderSystem>().Update(renderer, assetStore, camera);
  registry->GetSystem<RenderTextSystem>().Update(renderer, camera);
  registry->GetSystem<RenderHealthSystem>().Update(renderer, camera);
  if (isDebug)
  {
    registry->GetSystem<DebugRenderColliderSystem>().Update(renderer, camera);
    registry->GetSystem<RenderGUISystem>().Update(renderer, camera);

    //ImGui::ShowDemoWindow();
  }

  // ImGui::Render();
  // ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

  SDL_RenderPresent(renderer);
}

void Game::Destroy()
{
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  Logger::Log("Game Destroy!");
}
