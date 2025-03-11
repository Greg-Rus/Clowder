#pragma once
//#include "../Components/TextLabelComponent.h"
#include <SDL2/SDL.h>

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