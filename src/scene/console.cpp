#include <quest/scene/console.hpp>

#include <random>

#include <halcyon/utility/guard.hpp>

#include <quest/constants.hpp>
#include <quest/game.hpp>
#include <quest/helpers.hpp>

using namespace hq::scene;
using namespace hal::literals;

namespace hq::consts {
    constexpr std::string_view font_path { "assets/Ubuntu Mono.ttf" }, prefix_text { "root@Console ~ %" };

    constexpr hal::coord_t padding_pc { 0.015_crd };

    constexpr hal::color input_color { hal::palette::white },
        background_color { hal::palette::black, 128 },
        cursor_color { hal::palette::white, 128 },
        placeholder_color { 0x808080 },
        prefix_color { hal::palette::green };

    constexpr hal::coord::point text_offset { 10, 10 };

    constexpr hal::font::render_type text_render_type { hal::font::render_type::blended };

    constexpr bool clear_on_close { false };

    constexpr delta_t cursor_blink_time { 0.5 };

    constexpr std::size_t desired_max_chars { 128 };

    constexpr flag_bitmask enablers { flag::enable_draw, flag::enable_update, flag::block_process };
}

namespace {
    hal::pixel::point size_text(hal::ref<const hal::font> f, std::string_view t) {
        hal::text_engine::surface s;
        return s.make_text(f, t).size().get();
    }
}

console::shuffle_bag::shuffle_bag()
    : m_texts {
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
        "[40.7736N, 29.7564W]",
        "[sudo deez nuts]",
        "[docker? I barely know 'er!]",
        "[running out of time]",
        "[not actually random]",
        "[see you again]",
        "[forget me not]",
        "[one big CVE]"
    }
    , m_index { num_texts } {
}

hal::c_string console::shuffle_bag::next() {
    if (m_index == num_texts) { // Need to refresh.
        HAL_PRINT("<Console> Shuffling placeholder indices...");
        std::shuffle(std::begin(m_texts), std::end(m_texts), std::mt19937_64 { std::random_device {}() });
        m_index = 0;
    }

    return m_texts[m_index++];
}

console::console(game& g)
    : base { flag::enable_process }
    , m_font { find_sized_font(g.ttf, consts::font_path, static_cast<hal::pixel_t>(g.renderer.size()->y * 0.045)) }
    , m_padding { g.renderer.size()->x * consts::padding_pc }
    , m_texBegin { consts::text_offset.x + size_text(m_font, consts::prefix_text).x + m_padding }
    , m_wrap { static_cast<hal::pixel_t>(g.renderer.size()->x - m_texBegin - m_padding) }
    , m_outline { { m_texBegin, consts::text_offset.y }, size_text(m_font, " ") }
    , m_maxChars { static_cast<std::uint16_t>(std::min(g.renderer.info()->max_texture_size().x / m_outline.size.x, static_cast<hal::coord_t>(consts::desired_max_chars))) }
    , m_lineChars { static_cast<std::uint8_t>(m_wrap / m_outline.size.x) }
    , m_repaint { false }
    , m_cursorVis { true } {
    m_wrap -= m_wrap % static_cast<hal::pixel_t>(m_outline.size.x);

    HAL_WARN_IF(!m_font.mono(), '\"', m_font.family(), "\" is not a mono font. Character spacing will probably be incorrect.");
    HAL_PRINT("<Console> Initialized. Supports up to ", hal::to_printable_int(m_maxChars), " chars.");
}

void console::process(game& g) {
    for (const auto& evt : g.polled()) {
        switch (evt.kind()) {
            using enum hal::event::type;

        case key_pressed:
            if (process(g, evt.keyboard().key(), g.systems)) {
                if (flags.all(consts::enablers)) {
                    flags -= consts::enablers;
                    deactivate();
                } else {
                    flags += consts::enablers;
                    activate(g);
                }
            };
            break;

        case text_input:
            if (active())
                process(evt.text_input().text());
            break;

        default:
            break;
        }
    }
}

void console::update(game& g) {
    m_cursorTime += g.delta();

    if (m_cursorTime >= consts::cursor_blink_time) {
        m_cursorTime -= consts::cursor_blink_time;
        m_cursorVis = !m_cursorVis;
    }
}

void console::draw(game& g) {
    hal::lref<hal::renderer> rnd { g.renderer };

    hal::guard::color lock { rnd, consts::background_color };
    rnd->fill();

    if (m_repaint) {
        m_repaint = false;
        repaint(rnd);
    }

    rnd->draw(m_prefix).to(consts::text_offset)();

    hal::coord::point where { m_texBegin, consts::text_offset.y };

    hal::coord::rect crd;
    crd.size.x = static_cast<hal::coord_t>(std::min(m_line.size()->x, m_wrap));
    crd.size.y = m_outline.size.y;

    using namespace hal::literals;

    for (; m_line.size()->x - crd.pos.x > 0;
        where.y += m_outline.size.y, crd.pos.x += m_wrap, crd.size.x = std::min<hal::coord_t>(m_line.size()->x - crd.pos.x, static_cast<hal::coord_t>(m_wrap))) {
        rnd->draw(m_line).from(crd).to(where)();
    }

    if (m_cursorVis) {
        lock.set(consts::cursor_color);
        rnd->fill(m_outline);
    }
}

void console::activate(game& g) {
    m_repaint = true;

    m_cursorTime = 0.0;
    m_cursorVis  = true;

    m_prefix = { g.renderer, m_font.render(consts::prefix_text).fg(consts::prefix_color)(consts::text_render_type) };
}

void console::deactivate() {
    m_prefix.reset();
    m_line.reset();

    if constexpr (consts::clear_on_close) {
        m_field.text.clear();
    }
}

bool console::process(game& g, hal::keyboard::key k, hal::proxy::video vid) {
    switch (k) {
        using enum hal::keyboard::key;

    case F1:
        return true;

    case F12:
        if (flags[consts::enablers]) {
            g.screenshot = false; // Intercept screenshot requests.
        }
        break;

    default: {
        const field::op op { m_field.process(k, vid) };

        if (m_field.text.size() > m_maxChars) {
            m_field.trim(m_maxChars);
        }

        switch (op) {
            using enum field::op;

        case text_added:
        case text_removed:
            m_repaint = true;
            // Intentional fallthrough.

        case cursor_moved:
            set_cursor();
            break;

        default:
            break;
        }
    } break;
    };

    return false;
}

void console::process(std::string_view inp) {
    m_repaint = m_field.process(inp);

    if (m_field.text.size() > m_maxChars) {
        m_field.trim(m_maxChars);
    }

    m_repaint = true;

    set_cursor();
}

bool console::active() {
    return flags[consts::enablers];
}

void console::repaint(hal::lref<hal::renderer> rnd) {
    hal::surface text;

    if (m_field.text.empty()) {
        text = m_font.render(m_placeholders.next())
                   .fg(consts::placeholder_color)(consts::text_render_type);
    } else {
        text = m_font.render(m_field.text)
                   .fg(consts::input_color)(consts::text_render_type);
    }

    m_line = { rnd, text };
}

void console::set_cursor() {
    m_outline.pos.x = m_texBegin + (m_field.cursor % m_lineChars) * m_outline.size.x;
    m_outline.pos.y = consts::text_offset.y + m_outline.size.y * static_cast<std::uint8_t>(m_field.cursor / m_lineChars);

    m_cursorTime = consts::cursor_blink_time / 2.0;
    m_cursorVis  = true;
}
