#pragma once
#include "../ECS/ECS.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>
#include "../Components/CoreComponents.h"

class RenderGUISystem : public System
{
    std::unique_ptr<Registry> *registry;

public:
    RenderGUISystem(std::unique_ptr<Registry> &registry)
    {
        this->registry = &registry;
    }

    void Update(SDL_Renderer *renderer, SDL_Rect &camera)
    {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        char *names[] = {"tank-image", "truck-image", "chopper-image"};

        if (ImGui::Begin("Spawn Enemies"))
        {
            static int enemyXPos;
            static int enemyYPos;
            static int health;
            static int currentItem;
            ImGui::InputInt("Enemy X Position", &enemyXPos);
            ImGui::InputInt("Enemy Y Position", &enemyYPos);
            ImGui::InputInt("Health", &health);
            ImGui::Combo("Sprite", &currentItem, names, IM_ARRAYSIZE(names));
            ImGui::Spacing();

            if (ImGui::Button("Create enemy"))
            {
                Entity enemy = registry->get()->CreateEntity();
                enemy.Group("enemies");
                enemy.AddComponent<TransformComponent>(glm::vec2(enemyXPos, enemyYPos), glm::vec2(1.0, 1.0), 0.0);
                enemy.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
                enemy.AddComponent<SpriteComponent>(names[currentItem], 32, 32, 2);
                enemy.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));
                enemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(500, 0), 1000, 3000, false, 10);
                enemy.AddComponent<HealthComponent>(health);
                enemy.AddComponent<HealthDisplayComponent>("charriot-font");
            }
        }

        ImGui::End();

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0, 0));
        ImGui::SetNextWindowBgAlpha(0.6f);
        if (ImGui::Begin("Map Coordinates", NULL, windowFlags))
        {
            ImGui::Text(
                "Screen coordinates (x=%.1f, y=%.1f)",
                ImGui::GetIO().MousePos.x,
                ImGui::GetIO().MousePos.y);
            ImGui::Text(
                "Map coordinates (x=%.1f, y=%.1f)",
                ImGui::GetIO().MousePos.x + camera.x,
                ImGui::GetIO().MousePos.y + camera.y);
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    }
};