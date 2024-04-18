#pragma once

#include <quest/state/base.hpp>

namespace HQ::State
{
    class MainMenu : public Base
    {
    public:
        MainMenu() = default;

        Type Update(hal::event::handler& event, hal::f64 elapsed);
        void Draw(hal::video::renderer& rnd) const;
    };
}