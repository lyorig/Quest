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
            switch_state, // Switch this scene's active/parked state.

            nothing // Do nothing.
        };

        enum class flags {
            // Disabling flags:
            no_process,
            no_update,
            no_draw,

            // Blocker flags:
            stop_process,
            stop_draw,

            // Status flags:
            remove_me,

            max, // Only used for static asserts.

            // Combiner flags:
            all_disabling = no_process | no_update | no_draw,
            all_blocker   = stop_draw | stop_process,
            all_status    = remove_me
        };

        class base {
        public:
            // Alignment is gonna screw us over anyway, so might as well make these as big as possible.
            using flags_t = std::uintptr_t;

            // Paranoia.
            static_assert(std::to_underlying(flags::max) <= sizeof(flags_t) * CHAR_BIT);

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

            constexpr void switch_state() {
                if (flags[flags::all_disabling]) { // disabled
                    flags -= flags::all_disabling;
                } else { // enabled
                    flags += flags::all_disabling;
                }
            }

            hal::enum_bitset<flags, flags_t> flags;
        };
    }
}