#include <halcyon/image.hpp>
#include <quest/app.hpp>

int main(int argc, const char* argv[])
{
    HQ::App m_app { { argc, argv } };

    m_app.MainLoop();

    return EXIT_SUCCESS;
}