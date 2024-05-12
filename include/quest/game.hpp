#pragma once

#include <halcyon/events.hpp>
#include <halcyon/video.hpp>

#include <quest/state/base.hpp>

namespace HQ {
    class Arguments {
    public:
        Arguments(int argc, char** argv);

        bool operator[](std::string_view what) const;

    private:
        std::span<char*> m_span;
    };

    class Game {
    public:
        Game(Arguments args);

        void MainLoop();

    private:
        hal::context m_context;

        HAL_NO_SIZE hal::system::video m_video;
        HAL_NO_SIZE hal::ttf::context m_ttf;

        hal::window   m_window;
        hal::renderer m_renderer;

        hal::event::handler m_event;

        // Reference to the current state.
        std::unique_ptr<State::Base> m_state;
    };
}