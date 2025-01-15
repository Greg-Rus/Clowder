#pragma once
#include "../ECS/ECS.h"
#include "../Components/CoreComponents.h"
#include <SDL2/SDL.h>

class ProjectileEmitSystem : public System
{
    std::unique_ptr<Registry> *registry;

public:
    ProjectileEmitSystem(std::unique_ptr<Registry> &registry)
    {
        this->registry = &registry;
        RequireComponent<ProjectileEmitterComponent>();
        RequireComponent<TransformComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus)
    {
        eventBus->SubscribeToEvent<KeyPressedEvent>(this, &ProjectileEmitSystem::OnFireKeyPressed);
    }

    void OnFireKeyPressed(KeyPressedEvent &event)
    {
        if (event.key == SDLK_SPACE)
        {
            for (auto entity : GetSystemEntities())
            {
                if (entity.HasComponent<CameraFollowComponent>())
                {
                    SpawnProjectileIfReady(entity);
                }
            }
        }
    }

    void Update()
    {
        for (auto entity : GetSystemEntities())
        {
            if (entity.HasComponent<CameraFollowComponent>() == true)
            {
                continue;
            }

            SpawnProjectileIfReady(entity);
        }
    }

private:
    void SpawnProjectileIfReady(Entity &entity)
    {
        auto &projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
        const auto &transform = entity.GetComponent<TransformComponent>();

        if (SDL_GetTicks() - projectileEmitter.lastEmissionTime > projectileEmitter.repeatFrequency)
        {
            glm::vec2 spawnPoint = transform.position;
            if (entity.HasComponent<SpriteComponent>())
            {
                const auto &sprite = entity.GetComponent<SpriteComponent>();
                spawnPoint.x += (transform.scale.x * sprite.width / 2);
                spawnPoint.y += (transform.scale.y * sprite.height / 2);
            }

            glm::vec2 projectileVelocity = projectileEmitter.projectileVelocity;
            if (entity.HasComponent<RigidBodyComponent>())
            {
                auto &rigidBody = entity.GetComponent<RigidBodyComponent>();
                int directionX = 0;
                int directionY = 0;
                if (rigidBody.velocity.x > 0)
                    directionX = +1;
                if (rigidBody.velocity.x < 0)
                    directionX = -1;
                if (rigidBody.velocity.y > 0)
                    directionY = +1;
                if (rigidBody.velocity.y < 0)
                    directionY = -1;

                projectileVelocity.x = projectileEmitter.projectileVelocity.x * directionX;
                projectileVelocity.y = projectileEmitter.projectileVelocity.y * directionY;

                if(projectileVelocity == glm::vec2(0))
                {
                    projectileVelocity = projectileEmitter.projectileVelocity;
                }
            }

            Entity projectile = registry->get()->CreateEntity();
            projectile.Group("projectiles");
            projectile.AddComponent<TransformComponent>(spawnPoint, glm::vec2(1.0), 0.0);
            projectile.AddComponent<RigidBodyComponent>(projectileVelocity);
            projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
            projectile.AddComponent<BoxColliderComponent>(4, 4);
            projectile.AddComponent<ProjectileComponent>(
                projectileEmitter.isFriendly,
                projectileEmitter.hitPercentDamage,
                projectileEmitter.projectileDuration);

            projectileEmitter.lastEmissionTime = SDL_GetTicks();
        }
    }
};