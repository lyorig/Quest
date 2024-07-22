#pragma once

#include <halcyon/events.hpp>
#include <halcyon/video.hpp>

#include <quest/types.hpp>

// state/base.hpp:
// Base state class.

namespace hq {
    class game;

    namespace scene {
        enum class action : hal::u8 {
            kill,   // Remove this scene entirely.
            nothing // Do nothing.
        };

        using flags_t = std::uintptr_t;

        consteval flags_t make_flag(std::size_t index) {
            return static_cast<flags_t>(1) << index;
        }

        enum class flags : flags_t {
            // Enabling flags:
            enable_process = make_flag(0),
            enable_update  = make_flag(1),
            enable_draw    = make_flag(2),

            // Blocker flags:
            block_process = make_flag(7),
            block_update  = make_flag(8),
            block_draw    = make_flag(9),

            // Combiner flags:
            all_enable = enable_process | enable_update | enable_draw,
            all_block  = block_process | block_update | block_draw
        };

        using flag_bitmask = hal::enum_bitmask<flags>;

        class base {
        public:
            constexpr base(std::initializer_list<flags> f)
                : flags { f } {
            }

            virtual void process(game& g)         = 0;
            virtual void update(game& g)          = 0;
            virtual void draw(hal::renderer& rnd) = 0;

            virtual void activate(game& g) = 0;
            virtual void deactivate()      = 0;

            virtual std::string_view name() const = 0;

            virtual ~base() = default;

            flag_bitmask flags;
        };
    }
}
