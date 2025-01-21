 #pragma once
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"
#include "../Events/KeyUpEvent.h"
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

class KeyboardControlSystem : public System
{
    glm::vec2 inputDirection;
    int upInput = 0;
    int rightInput = 0;
    int downInput = 0;
    int leftInput = 0;
public:
    KeyboardControlSystem()
    {
        RequireComponent<KeyboardControlledComponent>();
        RequireComponent<SpriteComponent>();
        RequireComponent<RigidBodyComponent>();
        RequireComponent<OrientationComponent>();
        inputDirection = glm::vec2(0);
    }

    void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus)
    {
        eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnInputStart);
        eventBus->SubscribeToEvent<KeyUpEvent>(this, &KeyboardControlSystem::OnInputEnd);
    }

    void OnInputStart(KeyPressedEvent &event)
    {
        switch (event.key)
            {
            case SDLK_w:
            case SDLK_UP:
                upInput = -1;
                break;
            case SDLK_d:
            case SDLK_RIGHT:
                rightInput = 1;
                break;
            case SDLK_s:
            case SDLK_DOWN:
                downInput = 1;
                break;
            case SDLK_a:
            case SDLK_LEFT:
                leftInput = -1;
                break;

            default:
                break;
            }
        ProcessInput();
    }

        void OnInputEnd(KeyUpEvent &event)
    {
        switch (event.key)
            {
            case SDLK_w:
            case SDLK_UP:
                upInput = 0;
                break;
            case SDLK_d:
            case SDLK_RIGHT:
                rightInput = 0;
                break;
            case SDLK_s:
            case SDLK_DOWN:
                downInput = 0;
                break;
            case SDLK_a:
            case SDLK_LEFT:
                leftInput = 0;
                break;

            default:
                break;
            }
        ProcessInput();
    }

    void ProcessInput()
    {
        float horizontalInput = leftInput + rightInput;
        float verticalInput = upInput + downInput;
        if(horizontalInput != 0 && verticalInput != 0)
        {
            horizontalInput *= 0.71;
            verticalInput *= 0.71;
        }
        inputDirection = glm::vec2(horizontalInput, verticalInput);
    }

    // void OnKeyPressed(KeyPressedEvent &event)
    // {
    //     for (auto entity : GetSystemEntities())
    //     {
    //         // const auto keyoboardControl = entity.GetComponent<KeyboardControlledComponent>();
    //         // auto& sprite = entity.GetComponent<SpriteComponent>();
    //         // auto& rigidBody = entity.GetComponent<RigidBodyComponent>();
    //         // Logger::Log("Key Pressed: " + std::to_string(event.key));
    //         // switch (event.key)
    //         // {
    //         // case SDLK_w:
    //         // case SDLK_UP:
    //         //     rigidBody.velocity.y = keyoboardControl.upVelocity;
    //         //     sprite.srcRect.y = sprite.height * 0;
    //         //     Logger::Log("Up!!");
    //         //     break;
    //         // case SDLK_d:
    //         // case SDLK_RIGHT:
    //         //     rigidBody.velocity = keyoboardControl.rightVelocity;
    //         //     sprite.srcRect.y = sprite.height * 1;
    //         //     break;
    //         // case SDLK_s:
    //         // case SDLK_DOWN:
    //         //     rigidBody.velocity = keyoboardControl.downVelocity;
    //         //     sprite.srcRect.y = sprite.height * 2;
    //         //     break;
    //         // case SDLK_a:
    //         // case SDLK_LEFT:
    //         //     rigidBody.velocity = keyoboardControl.leftVelocity;
    //         //     sprite.srcRect.y = sprite.height * 3;
    //         //     break;

    //         // default:
    //         //     break;
    //         //}
    //     }
    // }

    // void OnKeyUp(KeyUpEvent &event)
    // {
        
    // }

    void Update()
    {
        for (auto entity : GetSystemEntities())
        {
            const auto keyoboardControl = entity.GetComponent<KeyboardControlledComponent>();
            auto& sprite = entity.GetComponent<SpriteComponent>();
            auto& rigidBody = entity.GetComponent<RigidBodyComponent>();
            auto& orientationComponent = entity.GetComponent<OrientationComponent>();
            rigidBody.velocity = inputDirection * keyoboardControl.velocity;

            if(inputDirection == glm::vec2(0))
            {
                return;
            }

            if(rightInput != 0)
            {
                sprite.srcRect.y = sprite.height * 1;
            }
            else if (leftInput !=0){
                sprite.srcRect.y = sprite.height * 3;
            }

            else if(upInput != 0)
            {
                sprite.srcRect.y = sprite.height * 0;
            }
            else{
                sprite.srcRect.y = sprite.height * 2;
            }
            orientationComponent.orientation = inputDirection;
        }
    }
};
