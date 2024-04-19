#include <quest/game.hpp>

#include <halcyon/utility/strutil.hpp>
#include <halcyon/utility/timer.hpp>

using namespace HQ;

Game::Game(Arguments args)
    : m_app { args }
    , m_state { new State::Intro { m_app.renderer, m_app.ttf } }
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

    case MainMenu:
        m_menu  = m_app;
        m_state = &m_menu;
        break;
    }

    return true;
}