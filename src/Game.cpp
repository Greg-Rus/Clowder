#include "Game.h"
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

Game::Game()
{
  isRunning = false;
  std::cout << "Game Constructor Called!" << std::endl;
}

Game::~Game() { std::cout << "Game Destructor Called!" << std::endl; }

void Game::Initialize()
{
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
  {
    std::cerr << "Failed to initialize SDL";
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
    std::cerr << "Failed to create SDL window";
    return;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer)
  {
    std::cerr << "Failed to create SDL renderer";
    return;
  }

  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
  isRunning = true;
  std::cout << "Game Initialized!" << std::endl;
}

glm::vec2 playerPositoin;
glm::vec2 playerVelocity;

void Game::Setup()
{
  playerPositoin = glm::vec2(10.0, 20.0);
  playerVelocity = glm::vec2(0.0, 0.0);
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
  std::cout << "Game Destroy!" << std::endl;
}

void Game::ProcessInput()
{
  SDL_Event sdlEvent;
  float playerSpeed = 50.0;
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
      if (sdlEvent.key.repeat == 0)
      {
        // Adjust the velocity
        switch (sdlEvent.key.keysym.sym)
        {
        case SDLK_UP:
          playerVelocity.y -= playerSpeed;
          break;
        case SDLK_DOWN:
          playerVelocity.y += playerSpeed;
          break;
        case SDLK_LEFT:
          playerVelocity.x -= playerSpeed;
          break;
        case SDLK_RIGHT:
          playerVelocity.x += playerSpeed;
          break;
        }
      }
      break;
    case SDL_KEYUP:
      if (sdlEvent.type == SDL_KEYUP && sdlEvent.key.repeat == 0)
      {
        // Adjust the velocity
        switch (sdlEvent.key.keysym.sym)
        {
        case SDLK_UP:
          playerVelocity.y += playerSpeed;
          break;
        case SDLK_DOWN:
          playerVelocity.y -= playerSpeed;
          break;
        case SDLK_LEFT:
          playerVelocity.x += playerSpeed;
          break;
        case SDLK_RIGHT:
          playerVelocity.x -= playerSpeed;
          break;
        }
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

  float deltaTime = (SDL_GetTicks() - millisecondsPreviousFrame) / 1000.0;

  millisecondsPreviousFrame = SDL_GetTicks();

  glm::vec2 scaledVelocity = playerVelocity;
  scaledVelocity *= deltaTime;
  playerPositoin += scaledVelocity;
}

void Game::Render()
{
  SDL_SetRenderDrawColor(renderer, 10, 100, 50, 255);
  SDL_RenderClear(renderer);

  SDL_Surface *surface = IMG_Load("./assets/images/tank-tiger-right.png");
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);

  SDL_Rect destinationRect = {static_cast<int>(playerPositoin.x),
                              static_cast<int>(playerPositoin.y),
                              32,
                              32};

  SDL_RenderCopy(renderer, texture, NULL, &destinationRect);

  SDL_DestroyTexture(texture);

  SDL_RenderPresent(renderer);
}
