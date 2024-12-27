#pragma once
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

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
            //transform.position.y += rigidBody.velocity.y;
            Logger::Log(
                "Entity id: " 
                +
                std::to_string(entity.GetId())
                +
                " position: "
                +
                std::to_string(transform.position.x)
                + "," +
                std::to_string(transform.position.y)
                );
        }
    }
};