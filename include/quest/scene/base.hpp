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
            kill,         // Remove this scene entirely.
            switch_state, // Switch this scene's active state.

            nothing // Do nothing.
        };

        using flags_t = std::uintptr_t;

        consteval flags_t make_flag(std::size_t index) {
            return static_cast<flags_t>(1) << index;
        }

        enum class flags : flags_t {
            // Enabling flags:
            update = make_flag(1),
            draw   = make_flag(2),

            // Blocker flags:
            stop_process = make_flag(3),
            stop_draw    = make_flag(4),

            // Status flags:
            remove_me = make_flag(5),

            // Combiner flags:
            all_enabling = update | draw,
            all_blocker  = stop_draw | stop_process,
            all_status   = remove_me
        };

        class base {
        public:
            constexpr base(std::initializer_list<flags> f)
                : flags { f } {
            }

            virtual action process(game& g)         = 0;
            virtual void   update(game& g)          = 0;
            virtual void   draw(hal::renderer& rnd) = 0;

            virtual void activate(game& g) = 0;
            virtual void deactivate()      = 0;

            virtual std::string_view name() const = 0;

            virtual ~base() = default;

            hal::enum_bitmask<flags, flags_t> flags;
        };
    }
}
