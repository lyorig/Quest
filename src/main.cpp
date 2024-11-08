#include <quest/game.hpp>

#include <halcyon/main.hpp>

int main(int argc, char* argv[]) {
    try {
        hq::game g { hq::args { argc, argv, std::nothrow } };

        g.main_loop();
    } catch (hal::exception) {
        std::puts(hal::debug::last_error());
    }

    return EXIT_SUCCESS;
}
