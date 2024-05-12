#pragma once

#include <halcyon/events.hpp>
#include <halcyon/video.hpp>

#include <quest/state/rest.hpp>

namespace HQ {
    class args {
    public:
        args(int argc, char** argv);

        bool operator[](std::string_view what) const;

    private:
        std::span<char*> m_span;
    };

    class game {
    public:
        game(args a);

        void main_loop();
        void quit();

    private:
        HAL_NO_SIZE hal::context m_context;

        HAL_NO_SIZE hal::system::video m_video;
        HAL_NO_SIZE hal::ttf::context m_ttf;

        hal::window   m_window;
        hal::renderer m_renderer;

        std::tuple<state::console> m_statics;

        hal::event::handler m_event;

        // Reference to the current state.
        std::unique_ptr<state::Base> m_state;

        // Whether the game is not quit yet.
        bool m_running { true };
    };
}