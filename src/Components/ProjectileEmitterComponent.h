#pragma once
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

struct ProjectileEmitterComponent
{
    glm::vec2 projectileVelocity;
    int repeatFrequency;
    int projectileDuration;
    bool isFriendly;
    int hitPercentDamage;
    int lastEmissionTime;

    ProjectileEmitterComponent(
        glm::vec2 projectileVelocity = glm::vec2(0),
        int repeatFrequency = 0,
        int projectileDuration = 10000,
        bool isFriendly = false,
        int hitPercentDamage = 10)
    {
        this->projectileVelocity = projectileVelocity;
        this->repeatFrequency = repeatFrequency;
        this->projectileDuration = projectileDuration;
        this->isFriendly = isFriendly;
        this->hitPercentDamage = hitPercentDamage;
        this->lastEmissionTime = SDL_GetTicks();
    }
};