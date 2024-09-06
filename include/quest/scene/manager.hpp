#pragma once

#include <tuple>

#include <quest/static_vector.hpp>

#include <quest/scene/console.hpp>
#include <quest/scene/dummy.hpp>
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

            manager(game& g);

            // Update the scene manager.
            void update(game& g);

        private:
            std::tuple<dummy> m_tuple;

            hal::u8 m_curr, m_cProcess, m_cUpdate, m_cDraw;

            template <typename T>
            void update_one(T& x, game& g);

            void update_cached(flag f);

            hal::u8 find_last_with_flag(flag m) const;
        };
    }
}
