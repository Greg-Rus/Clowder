#pragma once
#include "../ECS/ECS.h"
#include "../Components/CoreComponents.h"
#include <SDL2/SDL.h>

class DebugRenderColliderSystem : public System
{
    public:
        DebugRenderColliderSystem()
        {
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();
        }

        void Update(SDL_Renderer* renderer)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

            for(Entity e : GetSystemEntities())
            {
                const auto collider = e.GetComponent<BoxColliderComponent>();
                const auto transform = e.GetComponent<TransformComponent>();
                SDL_Rect box;
                box.x = transform.position.x + collider.offset.x;
                box.y = transform.position.y + collider.offset.y;
                box.w = collider.width * transform.scale.x;
                box.h = collider.height * transform.scale.y;

                SDL_RenderDrawRect(renderer, &box);
            }
        }
};