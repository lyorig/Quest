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

        using flags_t = std::uintptr_t;

        consteval flags_t make_flag(std::size_t index) {
            return static_cast<flags_t>(1) << index;
        }

        enum class flags : flags_t {
            // Disabling flags:
            no_process = make_flag(0),
            no_update  = make_flag(1),
            no_draw    = make_flag(2),

            // Blocker flags:
            stop_process = make_flag(3),
            stop_draw    = make_flag(4),

            // Status flags:
            remove_me = make_flag(5),

            // Combiner flags:
            all_disabling = no_process | no_update | no_draw,
            all_blocker   = stop_draw | stop_process,
            all_status    = remove_me
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

            constexpr void switch_state() {
                if (flags[flags::all_disabling]) { // disabled
                    flags -= flags::all_disabling;
                } else { // enabled
                    flags += flags::all_disabling;
                }
            }

            hal::enum_bitmask<flags, flags_t> flags;
        };
    }
}