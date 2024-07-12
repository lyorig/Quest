#include <quest/game.hpp>

int main(int argc, char* argv[]) {
    hq::game g { { argc, argv } };

    g.main_loop();

    return EXIT_SUCCESS;
}
