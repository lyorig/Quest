#pragma once

#include <halcyon/ttf.hpp>
#include <halcyon/video.hpp>

#include <quest/field.hpp>
#include <quest/types.hpp>

#include <quest/scenes/base.hpp>

namespace HQ {
    class shuffle_bag {
    public:
        static constexpr hal::u8 num_texts { 32 };

        shuffle_bag();

        const char* next();

    private:
        const char* m_arr[num_texts];

        hal::u8 m_index;
    };

    // A console. Designed for use with mono fonts.
    class console final : scene::base {
    public:
        console(hal::renderer& rnd, hal::ttf::context& ttf);

        scene::type update(game& g) override;

        bool process(hal::keyboard::key k, game& g);
        void process(std::string_view inp);

        void show(hal::renderer& rnd);
        void hide();

        // Is the console active?
        bool active();

    private:
        void repaint(hal::renderer& rnd);
        void set_cursor();

        shuffle_bag m_placeholders;

        hal::font m_font;

        field m_field;

        hal::texture m_pfx, m_tex;

        hal::coord_t m_padding;
        hal::coord_t m_texBegin;
        hal::pixel_t m_wrap;

        // pos = current outline pos
        // size = glyph size (constant)
        hal::coord::rect m_outline;

        hal::f64 m_cursorTime;

        hal::u16 m_maxChars;
        hal::u8  m_lineChars;

        bool m_active : 1, m_repaint : 1, m_cursorVis : 1;
    };
}