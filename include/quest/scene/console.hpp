#pragma once

#include <quest/atlas.hpp>
#include <quest/field.hpp>
#include <quest/scene/base.hpp>
#include <quest/types.hpp>

#include <halcyon/system.hpp>
#include <halcyon/ttf.hpp>
#include <halcyon/video.hpp>

namespace hq {
    // The placeholders are only necessary in the implementation, but their size
    // is also used for `m_placeholderOrder` -- oof!
    constexpr std::size_t NUM_PLACEHOLDERS { 38 + hal::platform::is_windows() };

    namespace scene {
        // A console. Designed for use with mono fonts.
        class console final : public base {
        public:
            console(game& g);

            void process(game& g);
            void update(game& g);
            void draw(game& g);

            void activate(game& g);
            void deactivate(game& g);

            // Process a keystroke.
            // Returns whether the console's state should be switched.
            bool process(game& g, hal::keyboard::key k, hal::proxy::video vid);

            // Process entered text.
            void process(std::string_view inp);

            // Returns whether the console is active.
            // This is detected via flags.
            bool is_active();

            // Write a string to the console.
            // This currently just calls `HAL_PRINT()`.
            void write(std::string_view text);

        private:
            // Places the cursor to an appropriate location on the screen.
            void set_cursor();

            hal::surface make_line();
            hal::surface make_placeholder();

            std::string_view generate_placeholder();

            // Parses `m_field` into a command + arguments, and attempts
            // to execute it.
            void execute_command(game& g);

            field m_field;

            std::uint8_t
                m_placeholderIndex,
                m_placeholderOrder[NUM_PLACEHOLDERS];

            hal::font m_font;

            texture_atlas::id
                m_prefix,
                m_line;

            hal::coord_t
                m_padding,
                m_texBegin;

            hal::pixel_t m_wrap;

            // pos = current outline pos
            // size = glyph size (constant)
            hal::coord::rect m_outline;

            double m_cursorTime;

            std::uint16_t m_maxChars;
            std::uint8_t  m_lineChars;

            bool
                m_repaint,   // Should we repaint?
                m_cursorVis; // Is the cursor visible?
        };

        static_assert(interface<console>);
    }
}
