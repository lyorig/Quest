#include <halcyon/image.hpp>
#include <quest/game.hpp>

int main(int argc, const char* argv[])
{
    HQ::Game m_app { { argc, argv } };

    m_app.MainLoop();

    return EXIT_SUCCESS;
}