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
        console(hal::font&& ttf);

        void draw(hal::renderer& rnd);

        void log(std::string&& str);

        void process(hal::keyboard::key k);
        void process(char ch);

        bool active() const;

        void show();
        void hide();

        void toggle();

    private:
        std::vector<std::string> m_entries;
        std::string              m_line;

        hal::font m_font;

        hal::texture     m_tex;
        hal::coord_point m_size;

        bool m_active, m_repaint;
    };
}