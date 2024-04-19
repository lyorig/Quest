#pragma once

#include <quest/animatable.hpp>
#include <quest/state/base.hpp>

namespace HQ::State
{
    class MainMenu : public Base
    {
    public:
        MainMenu() = default;
        MainMenu(const App& wnd);

        Type Update(App& event, hal::f64 elapsed);
        void Draw(hal::video::renderer& rnd) const;

    private:
        Animatable<hal::pixel_point, Easing::InOut::Parametric> m_wndSize, m_wndPos;
    };
}