#pragma once

#include <halcyon/audio.hpp>
#include <halcyon/video.hpp>

#include <quest/scene_manager.hpp>
#include <quest/scenes/console.hpp>
#include <quest/scenes/main_menu.hpp>

namespace HQ {
    class args {
    public:
        args(int argc, const char** argv);

        bool operator[](std::string_view what) const;

    private:
        std::span<const char*> m_span;
    };

    class game {
    public:
        game(args a);

        void main_loop();

        // Post a quit event.
        // This will be processed in the next event loop.
        void quit();

    private:
        HAL_NO_SIZE hal::context m_context;

        HAL_NO_SIZE hal::system::video m_video;
        HAL_NO_SIZE hal::system::audio m_audio;

        HAL_NO_SIZE hal::ttf::context m_ttf;
        HAL_NO_SIZE hal::image::context m_img;

        hal::window   m_window;
        hal::renderer m_renderer;

        hal::event::handler m_event;

        scene_manager m_sceneMgr;

        bool m_running;
    };
}