#pragma once

#include <halcyon/audio.hpp>
#include <halcyon/video.hpp>

#include <quest/atlas.hpp>
#include <quest/scene/manager.hpp>

namespace hq {
    class args {
    public:
        args(int argc, char** argv);

        bool operator[](std::string_view what) const;

    private:
        std::span<const char*> m_span;
    };

    class game {
    public:
        using event_vector = std::vector<hal::event::holder>;

        game(args a);

        void main_loop();

        const event_vector& polled() const;

        delta_t delta() const;

    private:
        hal::event::holder m_eventHandler;

        HAL_NO_SIZE hal::context m_context;

    public:
        HAL_NO_SIZE hal::system::video video;
        HAL_NO_SIZE hal::system::audio audio;

        HAL_NO_SIZE hal::ttf::context ttf;
        HAL_NO_SIZE hal::image::context img;

        hal::window   window;
        hal::renderer renderer;

        scene::manager scenes;
        atlas          atlas;

    private:
        event_vector m_polled;

        delta_t m_delta; // Intentionally uninitialized.

    public:
        delta_t timescale; // Multiplies the delta time that is given to scenes.

        bool running;    // Is the game running?
        bool screenshot; // Is a screenshot queued for this frame?

    private:
        void take_screenshot() const;
        void collect_events();
    };
}
