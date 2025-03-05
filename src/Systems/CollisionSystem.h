#pragma once

#include "../ECS/ECS.h"
#include "../Components/CoreComponents.h"
#include <glm/glm.hpp>
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"

class CollisionSystem : public System
{
public:
    CollisionSystem()
    {
        RequireComponent<BoxColliderComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update(std::unique_ptr<EventBus>& eventBus)
    {
        auto entities = GetSystemEntities();
        if (entities.size() <= 1)
        {
            return;
        }
        for (auto i = entities.begin(); i != entities.end(); i++)
        {
            Entity a = *i;
            for(auto j = i; j != entities.end(); j++)
            {
                Entity b = *j;
                if(a == b)
                {
                    continue;
                }
                bool collisionDetected = AreColliding(&*i, &*j);
                if(collisionDetected)
                {
                    // Logger::Log("Entity: " + 
                    // std::to_string(a.GetId()) +
                    // " collided with entity: " + 
                    // std::to_string(b.GetId()));
                    eventBus->EmitEvent<CollisionEvent>(a, b);
                }
            }
        }
    }

private:
    bool AreColliding(Entity* a, Entity* b)
    {
        glm::vec2 aPosition = a->GetComponent<TransformComponent>().position;
        glm::vec2 bPosition = b->GetComponent<TransformComponent>().position;
        glm::vec2 aScale = a->GetComponent<TransformComponent>().scale;
        glm::vec2 bScale = b->GetComponent<TransformComponent>().scale;
        BoxColliderComponent aBox = a->GetComponent<BoxColliderComponent>();
        BoxColliderComponent bBox = b->GetComponent<BoxColliderComponent>();

        glm::vec2 aOrigin = aPosition + aBox.offset;
        glm::vec2 bOrigin = bPosition + bBox.offset;

        return aOrigin.x < bOrigin.x + bBox.width * bScale.x &&
            aOrigin.x + aBox.width * aScale.x > bOrigin.x &&
            aOrigin.y < bOrigin.y + bBox.height * bScale.y &&
            aOrigin.y + aBox.height * aScale.y > bOrigin.y;
    }
};