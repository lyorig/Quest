#pragma once

#include <quest/atlas.hpp>
#include <quest/scene/manager.hpp>
#include <quest/thread_pool.hpp>

#include <halcyon/filesystem.hpp>
#include <halcyon/video.hpp>

#include <vector>

namespace hq {
    class args {
    public:
        using pos_t = std::int32_t;

        args(int argc, char** argv);

        pos_t size() const;

        struct info {
            constexpr static pos_t invalid_pos() {
                return lim<pos_t>::max();
            }

            pos_t pos;

            constexpr operator bool() const {
                return pos != invalid_pos();
            }

            constexpr bool operator!() const {
                return pos == invalid_pos();
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

        // Convenience atlas functions.
        texture_atlas::id    atlas_add(hal::surface s);
        void                 atlas_replace(texture_atlas::id id, hal::surface s);
        void                 atlas_pack();
        texture_atlas_copyer atlas_draw(texture_atlas::id id);

    private:
        hal::event::variant m_eventHandler;

    public:
        HAL_NO_SIZE hal::cleanup_init<hal::subsystem::video> systems;
        HAL_NO_SIZE hal::ttf::context ttf;

        hal::window   window;
        hal::renderer renderer;

        hal::fs::resource_loader loader;

        texture_atlas atlas;

        scene::manager scenes;

        thread_pool pool;

    private:
        event_vector m_polled;

    private:
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
