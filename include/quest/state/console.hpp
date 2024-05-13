#pragma once

#include <halcyon/ttf.hpp>
#include <halcyon/video.hpp>

#include <quest/animatable.hpp>
#include <quest/sprite.hpp>
#include <quest/text_field.hpp>

namespace HQ::state {
    // A console. Designed for use with mono fonts.
    class console {
    public:
        console(hal::ttf::context& ttf);

        void update(delta_t elapsed, hal::renderer& rnd);

        void process(hal::keyboard::key k, hal::keyboard::mod_state m);
        void process(char ch);

        void show(hal::renderer& rnd);
        void hide();

        // Is the console active?
        bool active();

        // Toggles and returns the new state.
        bool toggle(hal::renderer& rnd);

    private:
        void repaint(hal::renderer& rnd);

        static std::string_view random_placeholder_text();

        animatable<hal::color, easing::in::parabola<2>> m_col;

        hal::font m_font;

        text_field m_field;

        hal::texture m_pfx;
        hal::texture m_tex;
        hal::pixel_t m_texBegin;

        bool m_repaint;
    };
}