#pragma once

#include <quest/animatable.hpp>
#include <quest/state/base.hpp>

namespace HQ::State
{
    class MainMenu final : public Base
    {
    public:
        MainMenu(App& app);

        Type Update(App& event, hal::f64 elapsed);
        void Draw(hal::video::renderer& rnd) const;

    private:
        hal::color SwitchTheme();

        Animatable<hal::color, Easing::Linear> m_theme;

        bool m_currentTheme;
    };
}