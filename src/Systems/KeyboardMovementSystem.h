#pragma once
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"
#include <SDL2/SDL.h>

class KeyboardMovementSystem: public System
{
    public:
        KeyboardMovementSystem()
        {

        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
        {
            eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardMovementSystem::onKeyPressed);
        }

        void onKeyPressed(KeyPressedEvent& event)
        {
            switch (event.key)
            {
            case SDLK_w:
                Logger::Log("Up");
                break;
            case SDLK_a:
                Logger::Log("Left");
                break;
            case SDLK_s:
                Logger::Log("Down");
                break;
            case SDLK_d:
                Logger::Log("Right");
                break;
            
            default:
                break;
            }
        }

        void Update()
        {

        }
};
