#include <quest/game.hpp>

int main(int argc, char* argv[]) {
    hq::game g { { argc, argv } };

    g.main_loop();

    hal::cleanup();

    return EXIT_SUCCESS;
}
