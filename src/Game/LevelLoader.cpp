#include "LevelLoader.h"
#include "../ECS/ECS.h"
#include "../Components/CoreComponents.h"
#include "../Components/HealthDisplayComponent.h"
#include "../Game/Game.h"
#include <iostream>
#include <fstream>
#include <sstream>

LevelLoader::LevelLoader()
{
}

LevelLoader::~LevelLoader()
{
}

void LevelLoader::LoadLevel(int level, std::unique_ptr<Registry> &registry, std::unique_ptr<AssetStore> &assetStore, SDL_Renderer *renderer)
{
    assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
    assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
    assetStore->AddTexture(renderer, "bullet-image", "./assets/images/bullet.png");
    assetStore->AddTexture(renderer, "tree-image", "./assets/images/tree.png");
    assetStore->AddTexture(renderer, "jungle-map", "./assets/tilemaps/jungle.png");

    assetStore->AddFont("charriot-font", "./assets/fonts/charriot.ttf", 14);

    LoadTileMap("./assets/tilemaps/jungle.map", registry);

    Entity chopper = registry->CreateEntity();
    chopper.Tag("player");
    chopper.AddComponent<TransformComponent>(glm::vec2(10.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
    chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 2);
    chopper.AddComponent<AnimationComponent>(2, 10, true);
    chopper.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));
    int velocity = 200;
    chopper.AddComponent<KeyboardControlledComponent>(
        glm::vec2(0, -velocity),
        glm::vec2(velocity, 0),
        glm::vec2(0, velocity),
        glm::vec2(-velocity, 0),
        velocity);
    chopper.AddComponent<CameraFollowComponent>();
    chopper.AddComponent<HealthComponent>(100);
    chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(500, 500), 0, 3000, true, 10);
    chopper.AddComponent<HealthDisplayComponent>("charriot-font");
    chopper.AddComponent<OrientationComponent>(glm::vec2(1, 0));

    Entity radar = registry->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(Game::windowWidth - 64 - 10, 10), glm::vec2(1.0, 1.0), 0.0);
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2, true);
    radar.AddComponent<AnimationComponent>(8, 10, true);

    Entity tank = registry->CreateEntity();
    tank.Group("enemies");
    tank.AddComponent<TransformComponent>(glm::vec2(200.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(30.0, 0.0));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
    tank.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));
    tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(500, 0), 1000, 3000, false, 10);
    tank.AddComponent<HealthComponent>(100);
    tank.AddComponent<HealthDisplayComponent>("charriot-font");

    Entity truck = registry->CreateEntity();
    truck.Group("enemies");
    truck.AddComponent<TransformComponent>(glm::vec2(10.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
    truck.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));
    truck.AddComponent<HealthComponent>(100);
    truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(500, 0), 1000, 3000, false, 10);
    truck.AddComponent<HealthDisplayComponent>("charriot-font");

    Entity treeA = registry->CreateEntity();
    treeA.Group("obstacles");
    treeA.AddComponent<TransformComponent>(glm::vec2(300.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
    treeA.AddComponent<SpriteComponent>("tree-image", 16, 32, 2);
    treeA.AddComponent<BoxColliderComponent>(16, 32, glm::vec2(0));

    Entity treeB = registry->CreateEntity();
    treeB.Group("obstacles");
    treeB.AddComponent<TransformComponent>(glm::vec2(100.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
    treeB.AddComponent<SpriteComponent>("tree-image", 16, 32, 2);
    treeB.AddComponent<BoxColliderComponent>(16, 32, glm::vec2(0));

    Entity label = registry->CreateEntity();
    SDL_Color white = {255, 255, 255};
    label.AddComponent<TextLabelComponent>(glm::vec2(300, 300), "TEXT Test!!", "charriot-font", white, true);
}

void LevelLoader::LoadTileMap(const std::string &tileMapPath, std::unique_ptr<Registry> &registry)
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
  double tileScale = 3.0;
  int mapNumRows = map.size();
  int mapNumCols = map[0]->size();
  for (auto row = 0; row < mapNumRows; row++)
  {
    for (auto col = 0; col < mapNumCols; col++)
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
      tile.Group("tiles");
      tile.AddComponent<TransformComponent>(glm::vec2(tileX * tileScale, tileY * tileScale), glm::vec2(tileScale, tileScale), 0.0);
      tile.AddComponent<SpriteComponent>("jungle-map", 32, 32, 0, false, srcRectX, srcRectY);
    }
  }
  Game::mapWidth = mapNumCols * tileSize * tileScale;
  Game::mapHeight = mapNumRows * tileSize * tileScale;
}