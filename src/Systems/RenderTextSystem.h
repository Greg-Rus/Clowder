#pragma once
#include "../ECS/ECS.h"
#include "../Components/TextLabelComponent.h"
#include "../AssetStore/AssetStore.h"
#include <SDL2/SDL_ttf.h>

class RenderTextSystem : public System
{
    std::unique_ptr<AssetStore> *assetStore;

public:
    RenderTextSystem(std::unique_ptr<AssetStore> &assetStore)
    {
        this->assetStore = &assetStore;
        RequireComponent<TextLabelComponent>();
    }

    void Update(SDL_Renderer *renderer, const SDL_Rect &camera)
    {
        for (auto entity : GetSystemEntities())
        {
            const auto textLabel = entity.GetComponent<TextLabelComponent>();

            TTF_Font* font = assetStore->get()->GetFont(textLabel.assetId);

            SDL_Surface *surface = TTF_RenderText_Blended(
                font,
                textLabel.text.c_str(),
                textLabel.color);

            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            int labelWithd = 0;
            int labelHeight = 0;

            SDL_QueryTexture(texture, NULL, NULL, &labelWithd, &labelHeight);

            SDL_Rect dstRect = {
                static_cast<int>(textLabel.position.x - (textLabel.isFixed ? 0 : camera.x)),
                static_cast<int>(textLabel.position.y - (textLabel.isFixed ? 0 : camera.y)),
                labelWithd,
                labelHeight};

            SDL_RenderCopy(renderer, texture, NULL, &dstRect);
            SDL_DestroyTexture(texture);
        }
    }
};
