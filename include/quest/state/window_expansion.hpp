#pragma once

#include <quest/state/base.hpp>

#include <quest/animatable.hpp>

namespace HQ::State
{
    class WindowExpansion : public Base
    {
    public:
        WindowExpansion(const App& app, hal::pixel_point display_size);

        Type Update(App& event, hal::f64 elapsed);
        void Draw(hal::video::renderer& rnd) const;

    private:
        Animatable<hal::pixel_point, Easing::InOut::Bezier> m_wndSize, m_wndPos;
        Animatable<hal::color, Easing::InOut::Bezier>       m_drawCol;
    };
}