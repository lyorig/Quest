#include <quest/game.hpp>

#include <halcyon/utility/strutil.hpp>
#include <halcyon/utility/timer.hpp>

#include <quest/state/intro.hpp>
#include <quest/state/main_menu.hpp>
#include <quest/state/window_expansion.hpp>

using namespace HQ;

Game::Game(Arguments args)
    : m_app { args }
    , m_state { new State::Intro { m_app } }
{
}

void Game::MainLoop()
{
    hal::f64   delta;
    hal::timer timer;

    while (true)
    {
        delta = timer();
        timer.reset();

        if (!UpdateState(m_state->Update(m_app, delta)))
            break;

        m_state->Draw(m_app.renderer);

        m_app.Present();
    }
}

bool Game::UpdateState(State::Type tp)
{
    using enum State::Type;

    switch (tp)
    {
    case None:
        break;

    case Quit:
        return false;

    case WindowExpansion:
        m_state.reset(new State::WindowExpansion { m_app, m_app.video.displays[m_app.window.display_index()].size() });
        break;

    case MainMenu:
        m_state.reset(new State::MainMenu {});
        break;
    }

    return true;
}