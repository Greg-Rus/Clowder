#pragma once

struct SpriteComponent
{
    int width;
    int height;

    SpriteComponent(int width = 0, int height = 0)
    {
        this->width = width;
        this->height = height;
    }
};