#pragma once

#include <quest/atlas.hpp>
#include <quest/field.hpp>
#include <quest/scene/base.hpp>
#include <quest/types.hpp>

#include <halcyon/ttf.hpp>
#include <halcyon/video.hpp>

namespace hq {
    constexpr std::string_view console_placeholders[] {
        "[meow]",
        "[redacted]",
        "[your turn]",
        "[womp womp]",
        "[one big CVE]",
        "[kevin's heart]",
        "[lods of emone]",
        "[be not afraid]",
        "[see you again]",
        "[forget me not]",
        "[sudo deez nuts]",
        "[openest source]",
        "[at your service]",
        "[with eye serene]",
        "[is anyone there?]",
        "[food for thought]",
        "[made with Halcyon]",
        "[49.0481N, 17.4838E]",
        "[are you satisfied?]",
        "[enter command here]",
        "[running out of time]",
        "[not actually random]",
        "[watch?v=lo5cG0FhWro]",
        "[not POSIX compliant]",
        "[start typing, please]",
        "[commands not included]",
        "[segfaulting since 2021]",
        "[waiting for user input]",
        "[non-euclidean interface]",
        "[who needs documentation]",
        "[sudo pacman -S lyofetch]",
        "[no man page here, sorry]",
        "[ševalicious out tomorrow]",
        "[licensed under the WTFPL]",
        "[streets and sodium lights]",
        "[quoth the raven, nevermore]",
        "[docker? I barely know 'er!]",
        "[rm -rf / --no-preserve-root]",

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
