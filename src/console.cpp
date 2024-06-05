#include <quest/console.hpp>

#include <random>

#include <halcyon/utility/locks.hpp>

#include <quest/constants.hpp>
#include <quest/helpers.hpp>

using namespace HQ;

namespace HQ::consts {
    constexpr std::string_view font_path { "assets/Ubuntu Mono.ttf" }, prefix_text { "root@Console ~ %" };

    constexpr hal::pixel_t padding_left { 32 }, padding_right { 20 };

    constexpr hal::color input_color { hal::palette::white },
        background_color { hal::palette::black, 128 },
        cursor_color { hal::palette::white, 128 },
        placeholder_color { 0x808080 },
        prefix_color { hal::palette::green };

    constexpr hal::coord::point text_offset { 10, 10 };

    constexpr hal::font::render_type text_render_type { hal::font::render_type::blended };

    constexpr bool clear_on_close { false };
}

shuffle_bag::shuffle_bag()
    : m_arr {
        "[enter command here]",
        "[be not afraid]",
        "[food for thought]",
        "[waiting for user input]",
        "[rm -rf / --no-preserve-root]",
        "[at your service]",
        "[not POSIX compliant]",
        "[made with Halcyon]",
        "[start typing, please]",
        "[may 5th, 2022]",
        "[commands not included]",
        "[who needs documentation]",
        "[your turn]",
        "[segfaulting since 2021]",
        "[quoth the raven, nevermore]",
        "[sudo pacman -S neofetch]",
        "[redacted]",
        "[is anyone there?]",
        "[licensed under the WTFPL]",
        "[openest source]",
        "[watch?v=lo5cG0FhWro]",
        "[no man page here, sorry]",
        "[womp womp]",
        "[40.7736N, 29.7564W]",
        "[you can sudo, trust me]",
        "[docker? I barely know 'er!]",
        "[running out of time]",
        "[not actually random]",
        "[see you again]",
        "[forget me not]",
    }
    , m_index { num_texts } {
}

const char* shuffle_bag::next() {
    if (m_index == num_texts) { // Need to refresh.
        HAL_PRINT("<Console> Shuffling placeholder text...");
        std::shuffle(std::begin(m_arr), std::end(m_arr), std::mt19937_64 { std::random_device {}() });
        m_index = 0;
    }

    return m_arr[m_index++];
}

console::console(hal::renderer& rnd, hal::ttf::context& ttf)
    : m_font { find_sized_font(ttf, consts::font_path, static_cast<hal::pixel_t>(rnd.size().y * 0.045)) }
    , m_texBegin { consts::text_offset.x + m_font.size_text(consts::prefix_text).x + consts::padding_left }
    , m_wrap { static_cast<hal::pixel_t>(rnd.size().x - m_texBegin - consts::padding_right) }
    , m_outline { { m_texBegin, consts::text_offset.y }, m_font.size_text(" ") }
    , m_maxChars { static_cast<hal::u8>(m_wrap / m_outline.size.x) }
    , m_active { false }
    , m_repaint { false } {
}

void console::draw(hal::renderer& rnd) {
    hal::lock::color lock { rnd, consts::background_color };
    rnd.fill();

    if (m_repaint) {
        m_repaint = false;
        repaint(rnd);
    }

    auto pos = consts::text_offset;

    rnd.render(m_pfx).to(pos)();

    pos.x = m_texBegin;
    rnd.render(m_tex).to(pos)();

    lock.set(consts::cursor_color);

    rnd.fill(m_outline);
}

bool console::process(hal::keyboard::key k, hal::keyboard::mod_state m, const hal::proxy::clipboard& c) {
    switch (k) {
        using enum hal::keyboard::key;

    case F1:
        return true;

    default:
        m_repaint = m_field.process(k, m, c);
        break;
    };

    m_outline.pos.x = m_texBegin + (m_field.cursor % m_maxChars) * m_outline.size.x;
    m_outline.pos.y = consts::text_offset.y + m_outline.size.y * static_cast<hal::u8>(m_field.text.size() / m_maxChars);

    return false;
}

void console::process(std::string_view inp) {
    m_repaint = m_field.process(inp);

    m_outline.pos.x = m_texBegin + (m_field.cursor % m_maxChars) * m_outline.size.x;
    m_outline.pos.y = consts::text_offset.y + m_outline.size.y * static_cast<hal::u8>(m_field.text.size() / m_maxChars);
}

void console::show(hal::renderer& rnd) {
    m_repaint = true;
    m_active  = true;

    m_pfx = rnd.make_texture(m_font.render(consts::prefix_text).fg(consts::prefix_color)(consts::text_render_type));
}

void console::hide() {
    m_pfx.reset();
    m_tex.reset();

    if constexpr (consts::clear_on_close) {
        m_field.text.clear();
    }

    m_active = false;
}

bool console::active() {
    return m_active;
}

void console::repaint(hal::renderer& rnd) {
    hal::surface text;

    if (m_field.text.empty()) {
        text = m_font.render(m_placeholders.next())
                   .fg(consts::placeholder_color)(consts::text_render_type);
    } else {
        text = m_font.render(m_field.text)
                   .wrap(m_wrap)
                   .fg(consts::input_color)(consts::text_render_type);
    }

    m_tex = rnd.make_texture(text);
}