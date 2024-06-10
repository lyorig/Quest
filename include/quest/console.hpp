#pragma once

#include <halcyon/ttf.hpp>
#include <halcyon/video.hpp>

#include <quest/text_field.hpp>
#include <quest/types.hpp>

namespace HQ {
    class shuffle_bag {
    public:
        static constexpr hal::u8 num_texts { 30 };

        shuffle_bag();

        const char* next();

    private:
        const char* m_arr[num_texts];

        hal::u8 m_index;
    };

    // A console. Designed for use with mono fonts.
    class console {
    public:
        console() = default;
        console(hal::renderer& rnd, hal::ttf::context& ttf);

        void draw(hal::renderer& rnd);

        bool process(hal::keyboard::key k, hal::keyboard::mod_state m, const hal::proxy::clipboard& c);
        void process(std::string_view inp);

        void show(hal::renderer& rnd);
        void hide();

        // Is the console active?
        bool active();

    private:
        void repaint(hal::renderer& rnd);

        shuffle_bag m_placeholders;

        hal::font m_font;

        text_field m_field;

        hal::texture m_pfx, m_tex;

        hal::coord::point m_glyphSize;
        hal::pixel_t      m_texBegin, m_wrap;

        bool m_active : 1, m_repaint : 1;
    };
}