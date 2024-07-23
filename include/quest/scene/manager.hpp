#pragma once

#include <quest/static_vector.hpp>

#include <quest/scene/console.hpp>
#include <quest/scene/main_menu.hpp>

// scene_manager.hpp:
// A scene manager which aims to use as little space as possible. Here's hoping.

namespace hq {
    class game;

    namespace scene {
        // A LIFO scene manager.
        class manager {
        public:
            using base_up      = std::unique_ptr<scene::base>;
            using scene_vector = static_vector<base_up, 2>;

            using iterator       = scene_vector::iterator;
            using const_iterator = scene_vector::const_iterator;

            manager();

            // Update the scene manager.
            void update(game& g);

            // Add a scene.
            void add(base_up&& scn);

        private:
            scene_vector m_scenes;

            const_iterator m_begin, m_cachedProcess, m_cachedUpdate, m_cachedDraw;

            // Not const because static_vector is const correct or whatever.
            const_iterator find_last_with_flags(flag_bitmask m) const;

            void update_cached(flag f);
        };
    }
}
