#pragma once

#include <halcyon/utility/enum_bits.hpp>

#include <cstdint>

// state/base.hpp:
// Base state class.

namespace hq {
    class game;

    namespace scene {
        using flag_t = std::uintptr_t;

        consteval flag_t make_flag(std::size_t index) {
            return static_cast<flag_t>(1) << index;
        }

        enum class flag : flag_t {
            none = 0,

            // Enabling flags:
            enable_process = make_flag(0), // This scene can listen to events.
            enable_update  = make_flag(1), // This scene can perform delta updates.
            enable_draw    = make_flag(2), // This scene will be drawn each frame.

            // Blocker flags:
            block_process = make_flag(7), // Processing will terminate after this scene.
            block_update  = make_flag(8), // Updating will terminate after this scene.
            block_draw    = make_flag(9), // Drawing will terminate after this scene.

            // Combiner flags:
            all_enable = enable_process | enable_update | enable_draw,
            all_block  = block_process | block_update | block_draw
        };

        using flag_bitmask = hal::enum_bitmask<flag>;

        struct base {
            flag_bitmask flags;
        };

        // Used to check whether a class can be used in the manager.
        template <typename T>
        concept interface = std::is_base_of_v<base, T> && requires(T& scn, game& g) {
            scn.process(g);
            scn.update(g);
            scn.update(g);
        };
    }
}
