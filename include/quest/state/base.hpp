#pragma once

#include <halcyon/event.hpp>
#include <halcyon/video.hpp>

#include <quest/types.hpp>

// state/base.hpp:
// Base state class.

namespace HQ::State {
    class Base {
    public:
        virtual void  Process(const hal::event_handler& evt) = 0;
        virtual Base* Update(Delta elapsed)                  = 0;
        virtual void  Draw(hal::renderer& rnd) const         = 0;

        virtual ~Base() = default;
    };
}