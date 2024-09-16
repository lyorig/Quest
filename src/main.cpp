#include <quest/game.hpp>

int main(int argc, char* argv[]) {
    hq::game g { hq::args { argc, argv, std::nothrow } };

    g.main_loop();

    hal::cleanup();

    return EXIT_SUCCESS;
}
