#pragma once

#include <quest/state/base.hpp>

#include <quest/animatable.hpp>

namespace HQ::State {
    class WindowExpansion : public Base {
    public:
        WindowExpansion(const hal::window& wnd, const hal::system::video& vid, hal::renderer& rnd);

        constexpr static hal::color StartColor() {
            return hal::palette::white;
        }

        constexpr static hal::color EndColor() {
            return hal::palette::blue;
        }

        Type Update(App& event, hal::f64 elapsed);
        void Draw(hal::renderer& rnd) const;

    private:
        Animatable<hal::pixel_point, Easing::InOut::Bezier> m_wndSize, m_wndPos;
        Animatable<hal::color, Easing::InOut::Bezier>       m_drawCol;
    };
}