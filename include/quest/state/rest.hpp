#pragma once

#include <vector>

#include <quest/animatable.hpp>
#include <quest/sprite.hpp>

#include <quest/state/base.hpp>

// state/rest.hpp:
// The rest of the state types.

namespace HQ::state {
    class main_menu final : public Base {
    public:
        main_menu(hal::renderer& rnd, hal::ttf::context& ttf);

        void  Process(const hal::event::handler& event);
        Base* Update(hal::f64 elapsed);
        void  Draw(hal::renderer& rnd) const;

    private:
        hal::color SwitchTheme();

        animatable<hal::color, easing::in_out::bezier> m_theme;

        std::vector<sprite> m_widgets;

        hal::u8 m_currentTheme;
    };

    class console {
    public:
        console(hal::ttf::font&& ttf);

        void Draw(hal::renderer& rnd) const;

        void Log(std::string&& str);

        bool Active() const;
        void Toggle(hal::renderer& rnd);

        void Show(hal::renderer& rnd);
        void Hide();

    private:
        std::vector<std::string> m_entries;

        hal::ttf::font m_font;
        hal::texture   m_tex;

        bool m_active;
    };
}