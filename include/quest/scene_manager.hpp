#pragma once

#include <quest/static_vector.hpp>

#include <quest/scene/console.hpp>
#include <quest/scene/main_menu.hpp>

// scene_manager.hpp:
// A scene manager which aims to use as little space as possible. Here's hoping.

namespace HQ {
    class game;

    // A LIFO scene manager.
    class scene_manager {
    public:
        using base_up      = std::unique_ptr<scene::base>;
        using scene_vector = static_vector<base_up, 2>;

        scene_manager() = default;

        void update(game& g);

        // Add an active scene with priority.
        void add_active(base_up&& scn);

        // Add a parked (inactive) scene.
        void add_parked(base_up&& scn);

    private:
        scene_vector m_active, m_parked;

        // Where to start drawing from.
        scene_vector::iterator m_cachedLastOpaque, m_cachedLastProcess;

        scene_vector::iterator find_last_with_flag(scene::flags f);

        // Update cached iterators.
        void update_cached();
    };
}