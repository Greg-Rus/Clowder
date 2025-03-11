#include <iostream>
#include "Game/Game.h"
#include "../libs/sol/sol.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    Game game;
    game.Initialize();

    game.Run();

    game.Destroy();

    return 0;
}
