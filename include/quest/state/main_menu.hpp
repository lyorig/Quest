#pragma once

#include <quest/animatable.hpp>
#include <quest/sprite.hpp>
#include <quest/state/base.hpp>

namespace HQ::State {
    class MainMenu final : public Base {
    public:
        MainMenu(hal::renderer& rnd, hal::ttf::context& ttf);

        Type Update(App& event, hal::f64 elapsed);
        void Draw(hal::renderer& rnd) const;

    private:
        hal::color SwitchTheme();

        Animatable<hal::color, Easing::InOut::Bezier> m_theme;

        std::vector<Sprite> m_widgets;

        hal::u8 m_currentTheme;
    };
}