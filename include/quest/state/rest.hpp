#pragma once

#include <vector>

#include <quest/animatable.hpp>
#include <quest/sprite.hpp>

#include <quest/state/base.hpp>

// state/rest.hpp:
// The rest of the state types.

namespace HQ::State {
    class MainMenu final : public Base {
    public:
        MainMenu(hal::renderer& rnd, hal::ttf::context& ttf);

        void  Process(const hal::event_handler& evt);
        Base* Update(hal::f64 elapsed);
        void  Draw(hal::renderer& rnd) const;

    private:
        hal::color SwitchTheme();

        Animatable<hal::color, Easing::InOut::Bezier> m_theme;

        std::vector<Sprite> m_widgets;

        hal::u8 m_currentTheme;
    };
}