#pragma once

#include <halcyon/events.hpp>
#include <halcyon/video.hpp>

#include <quest/console.hpp>
#include <quest/state/main_menu.hpp>

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

        console m_console;

        hal::event::handler m_event;

        // Reference to the current state.
        std::unique_ptr<state::base> m_state;
    };
}