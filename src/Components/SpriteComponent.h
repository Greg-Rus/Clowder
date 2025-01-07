#pragma once
#include <string>
#include <SDL2/SDL.h>

struct SpriteComponent
{
    std::string assetId;
    int width;
    int height;
    SDL_Rect srcRect;
    int zIndex;
    bool isFixed;

    SpriteComponent(
        std::string assetId = "", 
        int width = 0, 
        int height = 0,
        int zIndex = 0,
        bool isFixed = false,
        int srcRectX = 0,
        int srcRectY = 0)
    {
        this->assetId = assetId;
        this->width = width;
        this->height = height;
        this->zIndex = zIndex;
        this->srcRect = {srcRectX, srcRectY, width, height};
        this->isFixed = isFixed;
    }
};