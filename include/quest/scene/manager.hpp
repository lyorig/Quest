#pragma once

#include <quest/scene/console.hpp>
#include <quest/scene/dummy.hpp>
#include <quest/scene/main_menu.hpp>

#include <tuple>

// scene/manager.hpp:
// A scene manager which aims to use as little space as possible. Here's hoping.

namespace hq {
    class game;

    namespace scene {
        // Holds and takes care of scenes.
        class manager {
        public:
            manager(game& g);

            // Performs the process-update-draw cycle.
            void update(game& g);

        private:
            std::tuple<main_menu, console> m_tuple;

            std::uint8_t
                m_curr,     // The scene currently being processed.
                m_cProcess, // Where to start processing.
                m_cUpdate,  // Where to start updating.
                m_cDraw;    // Where to start drawing.

            template <typename T>
            void update_one(T& x, game& g);

            void update_cached(flag f);

            std::uint8_t find_last_with_flag(flag m) const;
        };
    }
}
