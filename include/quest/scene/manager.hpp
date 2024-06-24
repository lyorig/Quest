#pragma once

#include <quest/static_vector.hpp>

#include <quest/scene/console.hpp>
#include <quest/scene/main_menu.hpp>

// scene_manager.hpp:
// A scene manager which aims to use as little space as possible. Here's hoping.
// It uses the PUD (Process, Update, Draw) model.

namespace hq {
    class game;

    namespace scene {
        // A LIFO scene manager.
        class manager {
        public:
            using base_up      = std::unique_ptr<scene::base>;
            using scene_vector = static_vector<base_up, 2>;
            using iterator     = scene_vector::iterator;

            manager() = default;

            // Update the scene manager.
            void update(game& g);

            // Add a scene.
            void add(base_up&& scn);

        private:
            void move(iterator from, iterator to);

            scene_vector m_scenes;
        };
    }
}