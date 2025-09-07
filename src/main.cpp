#include <quest/game.hpp>

#include <halcyon/main.hpp>

int main(int argc, char* argv[]) {
    hq::game g {
        hq::args { argc - 1, argv + 1 }
    };

    g.main_loop();

    return EXIT_SUCCESS;
}
