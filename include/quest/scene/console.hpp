#pragma once

#include <quest/atlas.hpp>
#include <quest/field.hpp>
#include <quest/scene/base.hpp>
#include <quest/types.hpp>

#include <halcyon/ttf.hpp>
#include <halcyon/video.hpp>

namespace hq {
    constexpr std::string_view console_placeholders[] {
        "[enter command here]",
        "[be not afraid]",
        "[food for thought]",
        "[waiting for user input]",
        "[rm -rf / --no-preserve-root]",
        "[at your service]",
        "[not POSIX compliant]",
        "[made with Halcyon]",
        "[start typing, please]",
        "[non-euclidean interface]",
        "[commands not included]",
        "[who needs documentation]",
        "[your turn]",
        "[segfaulting since 2021]",
        "[quoth the raven, nevermore]",
        "[sudo pacman -S lyofetch]",
        "[redacted]",
        "[is anyone there?]",
        "[licensed under the WTFPL]",
        "[openest source]",
        "[watch?v=lo5cG0FhWro]",
        "[no man page here, sorry]",
        "[womp womp]",
        "[49.0481N, 17.4838E]",
        "[sudo deez nuts]",
        "[docker? I barely know 'er!]",
        "[running out of time]",
        "[not actually random]",
        "[see you again]",
        "[forget me not]",
        "[one big CVE]",
        "[with eye serene]",
        "[ševalicious out tomorrow]",
        "[kevin's heart]",

#ifdef SDL_PLATFORM_WINDOWS
        "[MSVC is the real final boss]",
#endif
    };

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

            // Is the console active?
            bool active();

        private:
            void set_cursor();

            hal::surface make_line();
            hal::surface make_placeholder();

            std::string_view generate_placeholder();

            field m_field;

            std::uint8_t
                m_placeholderIndex,
                m_placeholderOrder[std::size(console_placeholders)];

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
