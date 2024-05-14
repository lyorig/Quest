#pragma once

#include <halcyon/ttf.hpp>
#include <halcyon/video.hpp>

#include <quest/text_field.hpp>
#include <quest/types.hpp>

namespace HQ {
    // A console. Designed for use with mono fonts.
    class console {
    public:
        console(hal::renderer& rnd, hal::ttf::context& ttf);

        void draw(hal::renderer& rnd);

        bool process(hal::keyboard::key k, hal::keyboard::mod_state m, const hal::proxy::clipboard& c);
        void process(char ch);

        void show(hal::renderer& rnd);
        void hide();

        // Is the console active?
        bool active();

        // Toggles and returns the new state.
        bool toggle(hal::renderer& rnd);

    private:
        void repaint(hal::renderer& rnd);

        hal::font m_font;

        text_field m_field;

        hal::texture m_pfx;
        hal::texture m_tex;
        hal::pixel_t m_texBegin, m_wrap;

        bool m_repaint;
    };
}