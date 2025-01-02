#pragma once

#include "../ECS/ECS.h"
#include "../Components/CoreComponents.h"
#include <glm/glm.hpp>

class CollisionSystem : public System
{
public:
    CollisionSystem()
    {
        RequireComponent<BoxColliderComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update()
    {
        auto entities = GetSystemEntities();
        if (entities.size() <= 1)
        {
            return;
        }
        for (int i = 0; i < entities.size() - 1; i++)
        {
            int nextEntityIndex = i + 1;

            for (int j = nextEntityIndex; j < entities.size(); j++)
            {
                AreColliding(entities[i], entities[j]);
            }
        }
    }

private:
    bool AreColliding(Entity a, Entity b)
    {
        glm::vec2 aPosition = a.GetComponent<TransformComponent>().position;
        glm::vec2 bPosition = b.GetComponent<TransformComponent>().position;
        BoxColliderComponent aBox = a.GetComponent<BoxColliderComponent>();
        BoxColliderComponent bBox = b.GetComponent<BoxColliderComponent>();

        glm::vec2 aOrigin = aPosition + aBox.offset;
        glm::vec2 bOrigin = bPosition + bBox.offset;

        if (aOrigin.x < bOrigin.x + bBox.width &&
            aOrigin.x + aBox.width > bOrigin.x &&
            aOrigin.y < bOrigin.y + bBox.height &&
            aOrigin.y + aBox.height > bOrigin.y)
        {
            Logger::Log("Collision!");
            return true;
        }
        else
        {
            return false;
        }
    }
};