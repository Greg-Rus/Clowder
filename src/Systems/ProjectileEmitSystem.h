#pragma once
#include "../ECS/ECS.h"
#include "../Components/CoreComponents.h"
#include <SDL2/SDL.h>

class ProjectileEmitSystem : public System
{
    public:
        ProjectileEmitSystem()
        {
            RequireComponent<ProjectileEmitterComponent>();
            RequireComponent<TransformComponent>();
        }

        void Update(std::unique_ptr<Registry>& registry)
        {
            for(auto entity: GetSystemEntities())
            {
                auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
                const auto& transform = entity.GetComponent<TransformComponent>();

                if(SDL_GetTicks() - projectileEmitter.lastEmissionTime > projectileEmitter.repeatFrequency)
                {
                    glm::vec2 spawnPoint = transform.position;
                    if(entity.HasComponent<SpriteComponent>())
                    {
                        const auto& sprite = entity.GetComponent<SpriteComponent>(); 
                        spawnPoint.x += (transform.scale.x * sprite.width / 2);
                        spawnPoint.y += (transform.scale.y * sprite.height / 2);
                    }
                    Entity projectile = registry->CreateEntity();
                    projectile.AddComponent<TransformComponent>(spawnPoint, glm::vec2(1.0), 0.0);
                    projectile.AddComponent<RigidBodyComponent>(projectileEmitter.projectileVelocity);
                    projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
                    projectile.AddComponent<BoxColliderComponent>(4, 4);
                    projectile.AddComponent<ProjectileComponent>(
                        projectileEmitter.isFriendly,
                        projectileEmitter.hitPercentDamage,
                        projectileEmitter.projectileDuration
                    );

                    projectileEmitter.lastEmissionTime = SDL_GetTicks();
                }
            }
        }
};