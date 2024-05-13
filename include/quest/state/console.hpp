#pragma once

#include <halcyon/ttf.hpp>
#include <halcyon/video.hpp>

#include <quest/sprite.hpp>
#include <quest/text_field.hpp>

namespace HQ::state {
    class console {
    public:
        console(hal::font&& ttf);

        void draw(hal::renderer& rnd);

        void process(hal::keyboard::key k, hal::keyboard::mod_state m);
        void process(char ch);

        void show();
        void hide();

        // Is the console active?
        bool active();

        // Returns the new state.
        bool toggle();

    private:
        text_field m_field;

        hal::font m_font;

        sprite m_tex;

        bool m_repaint;
    };
}