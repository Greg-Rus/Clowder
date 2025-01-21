#pragma once

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/HealthDisplayComponent.h"
#include "../Components/CoreComponents.h"
#include <stdlib.h>


class RenderHealthSystem : public System
{
    std::unique_ptr<AssetStore> *assetStore;

public:
    RenderHealthSystem(std::unique_ptr<AssetStore> &assetStore)
    {
        this->assetStore = &assetStore;
        RequireComponent<HealthDisplayComponent>();
        RequireComponent<TransformComponent>();
        RequireComponent<HealthComponent>();
        RequireComponent<SpriteComponent>();
    }

    void Update(SDL_Renderer *renderer, const SDL_Rect &camera)
    {
        for (auto entity : GetSystemEntities())
        {
            const auto healthDisplay = entity.GetComponent<HealthDisplayComponent>();
            const auto transformComponent = entity.GetComponent<TransformComponent>();
            const auto healthComponent = entity.GetComponent<HealthComponent>();
            const auto spriteComponent = entity.GetComponent<SpriteComponent>();

            TTF_Font* font = assetStore->get()->GetFont(healthDisplay.assetId);
            char text[10];
            SDL_itoa(healthComponent.healthPercentage, text, 10);
            SDL_Color color;
            if(healthComponent.healthPercentage >= 80)
            {
                color = {0, 255, 0};
            }
            else if(healthComponent.healthPercentage >= 30)
            {
                color = {255, 255, 0};
            }
            else 
            {
                color = {255, 0, 0};
            }

            SDL_Surface *surface = TTF_RenderText_Blended(
                font,
                text,
                color);

            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            int labelWithd = 0;
            int labelHeight = 0;

            SDL_QueryTexture(texture, NULL, NULL, &labelWithd, &labelHeight);

            SDL_Rect dstRect = {
                static_cast<int>(transformComponent.position.x - camera.x),
                static_cast<int>(transformComponent.position.y - camera.y - 25),
                labelWithd,
                labelHeight};

            SDL_RenderCopy(renderer, texture, NULL, &dstRect);
            SDL_DestroyTexture(texture);

            int barWidth = static_cast<int>(spriteComponent.width * (healthComponent.healthPercentage / 100.0));

            SDL_Rect healthBar;
            healthBar.x = transformComponent.position.x - camera.x;
            healthBar.y = transformComponent.position.y - camera.y - 10;
            healthBar.w = barWidth;
            healthBar.h = 5;

            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &healthBar);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        }
    }
};