#pragma once

#include <halcyon/ttf.hpp>
#include <halcyon/video.hpp>

#include <quest/field.hpp>
#include <quest/types.hpp>

#include <quest/scene/base.hpp>

namespace hq {
    namespace scene {
        // A console. Designed for use with mono fonts.
        class console final : public base {
        public:
            class shuffle_bag {
            public:
                static constexpr hal::u8 num_texts { 31 };

                shuffle_bag();

                const char* next();

            private:
                const char* m_texts[num_texts];

                hal::u8 m_index;
            };

            console(game& g);

            void process(game& g);
            void update(game& g);
            void draw(game& g);

            void activate(game& g);
            void deactivate();

            virtual std::string_view name() const;

            // Process a keystroke.
            // Returns whether the console's state should be switched.
            bool process(hal::keyboard::key k, const hal::proxy::video& vid);

            // Process entered text.
            void process(std::string_view inp);

            // Is the console active?
            bool active();

        private:
            void repaint(hal::renderer& rnd);
            void set_cursor();

            field m_field;

            shuffle_bag m_placeholders;

            hal::font m_font;

            hal::static_texture m_pfx, m_tex;

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
}
