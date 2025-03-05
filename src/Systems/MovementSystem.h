#pragma once
#include "../ECS/ECS.h"
#include "../Components/CoreComponents.h"
#include <glm/glm.hpp>
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"

class MovementSystem : public System
{
public:
    MovementSystem()
    {
        RequireComponent<TransformComponent>();
        RequireComponent<RigidBodyComponent>();
    }

    void Update(double deltaTime)
    {
        for (auto entity : GetSystemEntities())
        {
            auto &transform = entity.GetComponent<TransformComponent>();
            const auto &rigidBody = entity.GetComponent<RigidBodyComponent>();

            glm::vec2 nextPosition = transform.position + (rigidBody.velocity * glm::vec1(deltaTime));

            bool isEntityOutsideMap = nextPosition.x < 0 ||
                                      nextPosition.x > Game::mapWidth ||
                                      nextPosition.y < 0 ||
                                      nextPosition.y > Game::mapHeight;

            if (entity.HasTag("player"))
            {
                auto &sprite = entity.GetComponent<SpriteComponent>();
                int paddingLeft = 10;
                int paddingTop = 10;
                int paddingRigh = 10 + sprite.width;
                int paddingBottom = 10 + sprite.height;

                nextPosition.x = nextPosition.x < paddingLeft ? paddingLeft : nextPosition.x;
                nextPosition.x = nextPosition.x > Game::mapWidth - paddingRigh ? Game::mapWidth - paddingRigh : nextPosition.x;
                nextPosition.y = nextPosition.y < paddingTop ? paddingTop : nextPosition.y;
                nextPosition.y = nextPosition.y > Game::mapHeight - paddingBottom ? Game::mapHeight - paddingBottom : nextPosition.y;
            }

            if (isEntityOutsideMap && !entity.HasTag("player"))
            {
                entity.Kill();
            }

            transform.position = nextPosition;
        }
    }

    void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus)
    {
        eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::onCollision);
    }

    void onCollision(CollisionEvent &event)
    {
        Entity a = event.a;
        Entity b = event.b;
        if (a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles"))
        {
            OnEnemyhitsObstacle(a, b);
        }

        if (a.BelongsToGroup("obstacles") && b.BelongsToGroup("enemies"))
        {
            OnEnemyhitsObstacle(b, a);
        }
    }

    void OnEnemyhitsObstacle(Entity enemey, Entity obstacle)
    {
        if (enemey.HasComponent<RigidBodyComponent>() && enemey.HasComponent<SpriteComponent>())
        {
            auto &rigidbody = enemey.GetComponent<RigidBodyComponent>();
            auto &sprite = enemey.GetComponent<SpriteComponent>();

            if (rigidbody.velocity.x != 0)
            {
                rigidbody.velocity.x *= -1;
                sprite.flip = sprite.flip == SDL_FLIP_NONE ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            }

            if (rigidbody.velocity.y != 0)
            {
                rigidbody.velocity.y *= -1;
                sprite.flip = sprite.flip == SDL_FLIP_NONE ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
            }
        }
    }
};