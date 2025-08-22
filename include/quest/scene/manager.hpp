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
            // Also packs the game's texture atlas.
            void update(game& g);

        private:
            std::tuple<main_menu, console> m_tuple;

            std::uint8_t
                m_cProcess, // Where to start processing.
                m_cUpdate,  // Where to start updating.
                m_cDraw;    // Where to start drawing.

            // Do something for every scene in the tuple.
            // Takes care of updating and respecting starting positions.
            template <typename F>
            void for_each(std::uint8_t end, F func);

            // Find starting positions for all three update types.
            void update_cached(flag f);

            // Find the index of the last scene whose flags contain `m`.
            std::uint8_t find_last_with_flag(flag m) const;
        };
    }
}
