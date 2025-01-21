#pragma once
#include "../Components/TextLabelComponent.h"

struct HealthDisplayComponent
{
    static const SDL_Color healthyColor;
    static const SDL_Color damagedColor;
    static const SDL_Color dyingColor;

    std::string assetId;

    HealthDisplayComponent(std::string assetId =  "")
    {
        this->assetId = assetId;
    }
};

const SDL_Color HealthDisplayComponent::healthyColor = {0,255,0};
const SDL_Color HealthDisplayComponent::damagedColor = {0,255,255};
const SDL_Color HealthDisplayComponent::dyingColor = {255,0,0};