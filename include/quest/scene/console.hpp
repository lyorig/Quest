#pragma once

#include <quest/field.hpp>
#include <quest/scene/base.hpp>
#include <quest/types.hpp>

#include <halcyon/ttf.hpp>
#include <halcyon/video.hpp>

namespace hq {
    namespace scene {
        // A console. Designed for use with mono fonts.
        class console final : public base {
        private:
            class shuffle_bag {
            public:
                static constexpr std::uint8_t num_texts { 31 };

                shuffle_bag();

                hal::c_string next();

            private:
                hal::c_string m_texts[num_texts];

                std::uint8_t m_index;
            };

        public:
            console(game& g);

            void process(game& g);
            void update(game& g);
            void draw(game& g);

            void activate(game& g);
            void deactivate();

            // Process a keystroke.
            // Returns whether the console's state should be switched.
            bool process(game& g, hal::keyboard::key k, hal::proxy::video vid);

            // Process entered text.
            void process(std::string_view inp);

            // Is the console active?
            bool active();

        private:
            void repaint(hal::lref<hal::renderer> rnd);
            void set_cursor();

            field m_field;

            shuffle_bag m_placeholders;

            hal::font m_font;

            hal::static_texture m_prefix, m_line;

            hal::coord_t m_padding;
            hal::coord_t m_texBegin;
            hal::pixel_t m_wrap;

            // pos = current outline pos
            // size = glyph size (constant)
            hal::coord::rect m_outline;

            double m_cursorTime;

            std::uint16_t m_maxChars;
            std::uint8_t  m_lineChars;

            bool m_repaint, m_cursorVis;
        };

        static_assert(interface<console>);
    }
}
