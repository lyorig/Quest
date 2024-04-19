#pragma once

#include <quest/animatable.hpp>
#include <quest/state/base.hpp>

namespace HQ::State
{
    class MainMenu final : public Base
    {
    public:
        MainMenu() = default;

        Type Update(App& event, hal::f64 elapsed);
        void Draw(hal::video::renderer& rnd) const;
    };
}