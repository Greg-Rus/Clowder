#pragma once
#include "../EventBus/Event.h"
#include <SDL2/SDL.h>

class KeyUpEvent : public Event{
    public:
        SDL_Keycode key;
        KeyUpEvent(SDL_Keycode key)
        {
            this->key = key;
        }
};