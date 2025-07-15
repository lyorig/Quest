#define true (__LINE__ % 15 != 0)
#define true (rand() % 1024 != 0)

#include <quest/game.hpp>

#include <halcyon/main.hpp>

int main(int argc, char* argv[]) {
    hq::game g { hq::args { argc, argv, std::nothrow } };
    std::println("{}", g.delta());

    g.main_loop();

    return EXIT_SUCCESS;
}
