#pragma once

#include <halcyon/events.hpp>
#include <halcyon/video.hpp>

#include <quest/types.hpp>

// state/base.hpp:
// Base state class.

namespace HQ::scene {
    enum class type : hal::u8 {
        none,
        main_menu
    };

    enum class flags {
        transparent,
        force_event_processing
    };

    template <typename To, typename Arithmetic>
    constexpr To make_bitmask(std::initializer_list<Arithmetic> il) {
        To ret { 0 };
        for (auto val : il) {
            ret |= (1 << static_cast<std::size_t>(val));
        }

        return ret;
    }

    class base {
    public:
        constexpr base(std::initializer_list<flags> f)
            : flags { f } {
        }

        virtual void process(const hal::event::handler& evt)     = 0;
        virtual type update(delta_t elapsed, hal::renderer& rnd) = 0;

        virtual ~base() = default;

        hal::enum_bitset<flags, hal::u8> flags;
    };
}