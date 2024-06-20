#pragma once

#include <halcyon/ttf.hpp>
#include <halcyon/video.hpp>

#include <quest/field.hpp>
#include <quest/types.hpp>

#include <quest/scene/base.hpp>

namespace HQ {
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
                hal::u8 m_arr[num_texts], m_index;
            };

            console(game& g);

            action process(game& g) override;
            void   update(game& g) override;
            void   draw(hal::renderer& rnd) override;

            void activate(game& g) override;
            void deactivate() override;

            virtual std::string_view name() const override;

            // Process a keystroke.
            // Returns whether the console should be closed.
            bool process(hal::keyboard::key k, const hal::proxy::video& vid);

            // Process entered text.
            void process(std::string_view inp);

            // Is the console active?
            bool active();

        private:
            void repaint(hal::renderer& rnd);
            void set_cursor();

            field m_field; // 48

            shuffle_bag m_placeholders; // 32

            hal::font m_font; // 8

            hal::texture m_pfx, m_tex; // 8, 8

            hal::coord_t m_padding;  // 4
            hal::coord_t m_texBegin; // 4
            hal::pixel_t m_wrap;     // 4

            // pos = current outline pos
            // size = glyph size (constant)
            hal::coord::rect m_outline; // 16

            hal::f64 m_cursorTime; // 8

            hal::u16 m_maxChars;  // 2
            hal::u8  m_lineChars; // 1

            bool m_active : 1, m_repaint : 1, m_cursorVis : 1;
        };
    }
}