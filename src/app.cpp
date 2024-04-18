#include <quest/app.hpp>

#include <halcyon/utility/strutil.hpp>
#include <halcyon/utility/timer.hpp>

using namespace HQ;

Arguments::Arguments(int argc, const char* argv[])
    : m_span { argv, static_cast<std::size_t>(argc) }
{
}

bool Arguments::operator[](std::string_view what) const
{
    for (auto arg : m_span)
        if (hal::streq(arg, what.data()))
            return true;

    return false;
}

using enum hal::video::renderer::flags;

App::App(Arguments args)
    : m_video { m_context }
    , m_event { m_video.events }
    , m_window { m_video, "Amogus", { 640, 480 } }
    , m_renderer { m_window, { accelerated, vsync } }
    , m_state { new State::Intro { m_renderer, m_ttf } }
{
}

void App::MainLoop()
{
    hal::f64   delta;
    hal::timer timer;

    while (true)
    {
        delta = timer();
        timer.reset();

        if (!UpdateState(m_state->Update(m_event, delta)))
            break;

        m_state->Draw(m_renderer);

        m_renderer.present();
    }
}

bool App::UpdateState(State::Type tp)
{
    using enum State::Type;

    switch (tp)
    {
    case None:
        break;

    case Quit:
        return false;

    case MainMenu:
        m_state = &m_menu;
        break;
    }

    return true;
}