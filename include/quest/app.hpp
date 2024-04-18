#pragma once

#include <halcyon/event.hpp>
#include <halcyon/video.hpp>

#include <quest/state/intro.hpp>
#include <quest/state/main_menu.hpp>

namespace HQ
{
    class Arguments
    {
    public:
        Arguments(int argc, const char* argv[]);

        bool operator[](std::string_view what) const;

    private:
        std::span<const char*> m_span;
    };

    class App
    {
    public:
        App(Arguments args);

        void MainLoop();

    private:
        HAL_NO_SIZE hal::context m_context;
        HAL_NO_SIZE hal::ttf::context m_ttf;
        HAL_NO_SIZE hal::video::system m_video;

        hal::event::handler m_event;

        hal::video::window   m_window;
        hal::video::renderer m_renderer;

        // Reference to the current state.
        State::Base* m_state;

        State::MainMenu m_menu;

        bool UpdateState(State::Type tp);
    };
}