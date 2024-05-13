#pragma once

#include <halcyon/ttf.hpp>
#include <halcyon/video.hpp>

#include <quest/sprite.hpp>
#include <quest/text_field.hpp>

namespace HQ::state {

    // A console. Designed for use with mono fonts.
    class console {
    public:
        console(hal::ttf::context& ttf);

        void draw(hal::renderer& rnd);

        void process(hal::keyboard::key k, hal::keyboard::mod_state m);
        void process(char ch);

        void show();
        void hide();

        // Is the console active?
        bool active();

        // Toggles and returns the new state.
        bool toggle();

    private:
        void repaint(hal::renderer& rnd);

        hal::font m_font;

        text_field m_field;

        hal::surface m_pfx;
        hal::texture m_tex;

        bool m_repaint;
    };
}