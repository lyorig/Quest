#include <quest/game.hpp>

int main(int argc, char* argv[]) {
    HQ::Game game { { argc, argv } };

    game.MainLoop();

    return EXIT_SUCCESS;
}