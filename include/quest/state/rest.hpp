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

        void  process(const hal::event::handler& event) override;
        Base* update(hal::f64 elapsed) override;
        void  draw(hal::renderer& rnd) const override;

    private:
        hal::color switch_theme();

        animatable<hal::color, easing::in_out::bezier> m_theme;

        std::vector<sprite> m_widgets;

        hal::u8 m_currentTheme;
    };

    class console {
    public:
        console(hal::ttf::font&& ttf);

        void draw(hal::renderer& rnd) const;

        void log(std::string&& str);

        bool active() const;
        void toggle(hal::renderer& rnd);

        void show(hal::renderer& rnd);
        void hide();

    private:
        std::vector<std::string> m_entries;

        hal::ttf::font m_font;
        hal::texture   m_tex;

        bool m_active;
    };
}