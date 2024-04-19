#pragma once

#include <halcyon/event.hpp>

#include <quest/app.hpp>

#include <quest/state/base.hpp>

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
        std::unique_ptr<State::Base> m_state;

        bool UpdateState(State::Type tp);
    };
}