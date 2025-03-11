#pragma once
#include <memory>
#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"

class LevelLoader
{
public:
    LevelLoader();
    ~LevelLoader();
    void LoadLevel(int level, std::unique_ptr<Registry> &registry, std::unique_ptr<AssetStore> &assetStore, SDL_Renderer *renderer);

private:
    void LoadTileMap(const std::string &tileMapPath, std::unique_ptr<Registry> &registry);
};