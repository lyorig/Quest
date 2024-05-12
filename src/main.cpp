#include <quest/game.hpp>

int main(int argc, char* argv[]) {
    HQ::game game { { argc, argv } };

    game.main_loop();

    return EXIT_SUCCESS;
}