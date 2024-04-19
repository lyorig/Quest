#pragma once

#include <halcyon/event.hpp>

#include <quest/app.hpp>

#include <quest/state/intro.hpp>
#include <quest/state/main_menu.hpp>

namespace HQ
{
    class Game
    {
    public:
        Game(Arguments args);

        void MainLoop();

    private:
        App m_app;

        // Reference to the current state.
        State::Base* m_state;

        State::MainMenu m_menu;

        bool UpdateState(State::Type tp);
    };
}