#pragma once
#include "../EventBus/Event.h"
#include <SDL2/SDL.h>

class KeyPressedEvent : public Event{
    public:
        SDL_Keycode key;
        KeyPressedEvent(SDL_Keycode key)
        {
            this->key = key;
        }
};