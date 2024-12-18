#pragma once

#include "halcyon/types/c_string.hpp"
#include <halcyon/audio.hpp>
#include <halcyon/video.hpp>

#include <halcyon/image.hpp>

#include <limits>
#include <quest/scene/manager.hpp>

namespace hq {
    class args {
    public:
        using pos_t = std::int32_t;

        args(int argc, char** argv);
        args(int argc, char** argv, std::nothrow_t);

        pos_t size() const;

        struct info {
            consteval static pos_t invalid_pos() {
                return std::numeric_limits<pos_t>::max();
            }

            pos_t pos;

            constexpr operator bool() const {
                return pos != invalid_pos();
            }
        };

        info          operator[](std::string_view what) const;
        hal::c_string operator[](pos_t pos) const;

    private:
        std::span<const char*> m_span;
    };

    class game {
    public:
        using event_vector = std::vector<hal::event::variant>;

        game(args a);

        void main_loop();

        const event_vector& polled() const;

        delta_t delta() const;

    private:
        hal::event::variant m_eventHandler;

    public:
        HAL_NO_SIZE hal::cleanup_init<hal::subsystem::video, hal::subsystem::audio> systems;

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
