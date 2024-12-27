#pragma once
#include "../ECS/ECS.h"
#include "../Components/CoreComponents.h"
#include <glm/glm.hpp>

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
        for (auto entity: GetSystemEntities())
        {
            auto& transform = entity.GetComponent<TransformComponent>();
            const auto& rigidBody = entity.GetComponent<RigidBodyComponent>();
            
            transform.position += (rigidBody.velocity * glm::vec1(deltaTime));
        }
    }
};