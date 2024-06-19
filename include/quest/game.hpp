#pragma once

#include <halcyon/audio.hpp>
#include <halcyon/video.hpp>

#include <quest/scene_manager.hpp>

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
        using event_vector = std::vector<hal::event::handler>;

        game(args a);

        void main_loop();
        void quit();

        const event_vector& polled() const;

        delta_t delta() const;

    private:
        HAL_NO_SIZE hal::context m_context;

    public:
        HAL_NO_SIZE hal::system::video video;
        HAL_NO_SIZE hal::system::audio audio;

        HAL_NO_SIZE hal::ttf::context ttf;
        HAL_NO_SIZE hal::image::context img;

        hal::window   window;
        hal::renderer renderer;

    private:
        scene_manager m_scenes;
        event_vector  m_polled;

        delta_t m_delta;
        bool    m_running;

        void collect_events();
    };
}