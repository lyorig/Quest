#pragma once

#include <halcyon/events.hpp>
#include <halcyon/video.hpp>

#include <quest/types.hpp>

// state/base.hpp:
// Base state class.

namespace HQ {
    class game;

    namespace scene {
        enum class type : hal::u8 {
            none,
            main_menu
        };

        enum class action : hal::u8 {
            none,
            kill,
            prioritize,
            activate,
            deactivate
        };

        enum class flags {
            transparent,
            force_event_processing
        };

        class base {
        public:
            constexpr base(std::initializer_list<flags> f)
                : flags { f } {
            }

            virtual type update(game& g) = 0;

            virtual ~base() = default;

            hal::enum_bitset<flags, hal::u8> flags;
        };
    }
}