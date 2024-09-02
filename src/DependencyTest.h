#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <sol/sol.hpp>

int TestDependencies()
{
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    glm::vec2 velocity = glm::vec2(0.5, 1);
    velocity = glm::normalize(velocity);

    SDL_Init(SDL_INIT_EVERYTHING);

    std::cout << "Meow, meow, everything works!" << std::endl;

    return 0;
}