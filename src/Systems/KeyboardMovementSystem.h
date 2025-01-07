 #pragma once
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"
#include <SDL2/SDL.h>

class KeyboardControlSystem : public System
{
public:
    KeyboardControlSystem()
    {
        RequireComponent<KeyboardControlledComponent>();
        RequireComponent<SpriteComponent>();
        RequireComponent<RigidBodyComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus)
    {
        eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
    }

    void OnKeyPressed(KeyPressedEvent &event)
    {
        for (auto entity : GetSystemEntities())
        {
            const auto keyoboardControl = entity.GetComponent<KeyboardControlledComponent>();
            auto& sprite = entity.GetComponent<SpriteComponent>();
            auto& rigidBody = entity.GetComponent<RigidBodyComponent>();
            Logger::Log("Key Pressed: " + std::to_string(event.key));
            switch (event.key)
            {
            case SDLK_w:
            case SDLK_UP:
                rigidBody.velocity = keyoboardControl.upVelocity;
                sprite.srcRect.y = sprite.height * 0;
                Logger::Log("Up!!");
                break;
            case SDLK_d:
            case SDLK_RIGHT:
                rigidBody.velocity = keyoboardControl.rightVelocity;
                sprite.srcRect.y = sprite.height * 1;
                break;
            case SDLK_s:
            case SDLK_DOWN:
                rigidBody.velocity = keyoboardControl.downVelocity;
                sprite.srcRect.y = sprite.height * 2;
                break;
            case SDLK_a:
            case SDLK_LEFT:
                rigidBody.velocity = keyoboardControl.leftVelocity;
                sprite.srcRect.y = sprite.height * 3;
                break;

            default:
                break;
            }
        }
    }

    void Update()
    {
    }
};
