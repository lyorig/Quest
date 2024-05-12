#pragma once

#include <halcyon/events.hpp>
#include <halcyon/video.hpp>

#include <quest/types.hpp>

// state/base.hpp:
// Base state class.

namespace HQ::state {
    class Base {
    public:
        virtual void  process(const hal::event::handler& evt) = 0;
        virtual Base* update(delta_t elapsed)                 = 0;
        virtual void  draw(hal::renderer& rnd) const          = 0;

        virtual ~Base() = default;
    };
}