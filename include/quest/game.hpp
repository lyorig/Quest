#pragma once

#include <halcyon/audio.hpp>
#include <halcyon/video.hpp>

#include <halcyon/image.hpp>

#include <quest/scene/manager.hpp>

namespace hq {
    class args {
    public:
        args(int argc, char** argv);
        args(int argc, char** argv, std::nothrow_t);

        std::size_t size() const;

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

    public:
        HAL_NO_SIZE hal::cleanup_init<hal::system::video, hal::system::audio> systems;

        HAL_NO_SIZE hal::ttf::context ttf;
        HAL_NO_SIZE hal::image::context img;

        hal::window   window;
        hal::renderer renderer;

        scene::manager scenes;

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
