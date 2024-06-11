#pragma once

#include <halcyon/events.hpp>
#include <halcyon/video.hpp>

#include <quest/types.hpp>

// state/base.hpp:
// Base state class.

namespace HQ::state {
    class base {
    public:
        virtual void  process(const hal::event::handler& evt)     = 0;
        virtual base* update(delta_t elapsed, hal::renderer& rnd) = 0;

        virtual ~base() = default;
    };
}