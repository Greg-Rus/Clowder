#pragma once
#include "../ECS/ECS.h"
#include "../Components/CoreComponents.h"
#include <SDL2/SDL.h>
#include "../AssetStore/AssetStore.h"
#include <algorithm>

class RenderSystem : public System
{
public:
    RenderSystem()
    {
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    }

    void Update(
    SDL_Renderer *renderer, 
    std::unique_ptr<AssetStore>& assetStore,
    SDL_Rect& camera)
    {
        struct RenderableEntitiy{
            TransformComponent transformComponent;
            SpriteComponent spriteComponent;
        };

        std::vector<RenderableEntitiy> entities;
        for (auto entity : GetSystemEntities())
        {
            RenderableEntitiy renderableEntitiy;
            renderableEntitiy.transformComponent = entity.GetComponent<TransformComponent>();
            renderableEntitiy.spriteComponent = entity.GetComponent<SpriteComponent>();

            bool isEntityOutsideCmaeraView = (
                renderableEntitiy.transformComponent.position.x + (renderableEntitiy.spriteComponent.width * renderableEntitiy.transformComponent.scale.x) < camera.x ||
                renderableEntitiy.transformComponent.position.x > camera.x + camera.w ||
                renderableEntitiy.transformComponent.position.y + (renderableEntitiy.spriteComponent.height * renderableEntitiy.transformComponent.scale.y) < camera.y ||
                renderableEntitiy.transformComponent.position.y > camera.y + camera.h
            );
            if(isEntityOutsideCmaeraView)
            {
                continue;
            }
            entities.emplace_back(renderableEntitiy);
        }

        std::sort(
            entities.begin(), 
            entities.end(), 
            [](const RenderableEntitiy& a, const RenderableEntitiy& b)
            {
                return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
            }
            );

        for (auto entity : entities)
        {
            const auto &transform = entity.transformComponent;
            const auto &sprite = entity.spriteComponent;

            SDL_Rect srcRect = sprite.srcRect;

            SDL_Rect dstRect = {
                static_cast<int>(transform.position.x - (sprite.isFixed ? 0 : camera.x)),
                static_cast<int>(transform.position.y - (sprite.isFixed ? 0 : camera.y)),
                static_cast<int>(sprite.width * transform.scale.x),
                static_cast<int>(sprite.height * transform.scale.y)
            };

            SDL_RenderCopyEx(
                renderer,
                assetStore->GetTexture(sprite.assetId),
                &srcRect,
                &dstRect,
                transform.rotation,
                NULL,
                sprite.flip
            );
        }
    }
};