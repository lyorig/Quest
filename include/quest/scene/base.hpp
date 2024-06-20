#pragma once

#include <halcyon/events.hpp>
#include <halcyon/video.hpp>

#include <quest/types.hpp>

// state/base.hpp:
// Base state class.

namespace HQ {
    class game;

    namespace scene {
        enum class action : hal::u8 {
            none,        // Do nothing.
            switch_state // (De)activate this state.
        };

        enum class flags {
            opaque,                  // Drawing should stop here.
            block_further_processing // Event processing should stop here.
        };

        class base {
        public:
            constexpr base(std::initializer_list<flags> f)
                : flags { f } {
            }

            virtual action process(const std::vector<hal::event::handler>& vector, const hal::proxy::video& vid) = 0;

            virtual void update(delta_t elapsed)  = 0;
            virtual void draw(hal::renderer& rnd) = 0;

            virtual void activate(game& g) = 0;
            virtual void deactivate()      = 0;

            virtual std::string_view name() const = 0;

            virtual ~base() = default;

            hal::enum_bitset<flags, hal::u8> flags;
        };
    }
}