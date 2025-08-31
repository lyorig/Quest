#include <quest/scene/console.hpp>

#include <halcyon/utility/guard.hpp>

#include <quest/constants.hpp>
#include <quest/game.hpp>
#include <quest/helpers.hpp>

#include <numeric>
#include <random>

using namespace hq::scene;
using namespace hal::literals;

namespace hq::consts {
    constexpr std::string_view prefix_text { "raine1@Arctic~ %" };

    constexpr hal::coord_t padding_pc { 0.015_crd };

    constexpr hal::color input_color { hal::colors::white },
        background_color { hal::colors::black, 128 },
        cursor_color { hal::colors::white, 128 },
        placeholder_color { 0x808080 },
        prefix_color { hal::colors::green };

    constexpr hal::coord::point text_offset { 10, 10 };

    constexpr delta_t cursor_blink_time { 0.5 };

    constexpr std::size_t desired_max_chars { 128 };

    constexpr flag_bitmask enablers { flag::enable_draw, flag::enable_update, flag::block_process };
}

console::console(game& g)
    : base { flag::enable_process }
    , m_placeholderIndex { std::size(console_placeholders) }
    , m_font { find_sized_font(g, "assets/Ubuntu Mono.ttf", static_cast<hal::pixel_t>(g.renderer.size()->y * 0.045)) }
    , m_padding { g.renderer.size()->x * consts::padding_pc }
    , m_texBegin { consts::text_offset.x + size_text(m_font, consts::prefix_text).x + m_padding }
    , m_wrap { static_cast<hal::pixel_t>(g.renderer.size()->x - m_texBegin - m_padding) }
    , m_outline { { m_texBegin, consts::text_offset.y }, size_text(m_font, " ") }
    , m_maxChars { static_cast<std::uint16_t>(std::min(g.renderer.props().max_texture_size() / m_outline.size.x, static_cast<hal::coord_t>(consts::desired_max_chars))) }
    , m_lineChars { static_cast<std::uint8_t>(m_wrap / m_outline.size.x) }
    , m_repaint { false }
    , m_cursorVis { true } {
    m_wrap -= m_wrap % static_cast<hal::pixel_t>(m_outline.size.x);

    std::iota(std::begin(m_placeholderOrder), std::end(m_placeholderOrder), 0);

    HAL_WARN_IF(!m_font.mono(), "<Console> \"", m_font.family(), "\" is not a mono font. Character spacing will probably be incorrect.");
}

void console::process(game& g) {
    for (const auto& evt : g.polled()) {
        switch (evt.kind()) {
            using enum hal::event::type;

        case key_pressed:
            if (process(g, evt.keyboard().key(), g.systems)) {
                if (flags.all(consts::enablers)) {
                    flags -= consts::enablers;
                    deactivate(g);
                } else {
                    flags += consts::enablers;
                    activate(g);
                }
            };
            break;

        case text_input:
            if (active()) {
                process(evt.text_input().text());
            }

            break;

        default:
            break;
        }
    }
}

void console::activate(game& g) {
    m_cursorTime = 0.0;
    m_cursorVis  = true;

    m_prefix = g.atlas_add(m_font.render_blended(consts::prefix_text, consts::prefix_color));
    m_line   = g.atlas_add(make_placeholder());

    g.systems.events.text_input_start(g.window);
}

void console::deactivate(game& g) {
    g.systems.events.text_input_stop(g.window);

    g.atlas.free(m_prefix);
    g.atlas.free(m_line);

    m_field.clear();
    set_cursor();
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
        const field::action op { m_field.process(k, vid) };

        if (m_field.text.size() > m_maxChars) {
            m_field.trim(m_maxChars);
        }

        switch (op) {
            using enum field::action;

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

    g.atlas_draw(m_prefix).to(consts::text_offset).render();

    hal::coord::point where { m_texBegin, consts::text_offset.y };

    hal::coord::rect   crd;
    const hal::pixel_t m_line_size_x = g.atlas.area(m_line).size.x;

    crd.size.x = static_cast<hal::coord_t>(std::min(m_line_size_x, m_wrap));
    crd.size.y = m_outline.size.y;

    using namespace hal::literals;

    for (; m_line_size_x - crd.pos.x > 0;
        where.y += m_outline.size.y,
        crd.pos.x += m_wrap,
        crd.size.x = std::min<hal::coord_t>(m_line_size_x - crd.pos.x, static_cast<hal::coord_t>(m_wrap))) {
        g.atlas_draw(m_line).from(crd).to(where).render();
    }

    if (m_cursorVis) {
        lock.set(consts::cursor_color);
        rnd->fill(m_outline);
    }

    if (m_repaint) {
        m_repaint = false;
        g.atlas_replace(m_line, make_line());
    }
}

void console::process(std::string_view inp) {
    m_repaint = m_field.process(inp);

    if (m_field.text.size() > m_maxChars) {
        m_field.trim(m_maxChars);
    }

    set_cursor();
}

bool console::active() {
    return flags[consts::enablers];
}

hal::surface console::make_line() {
    if (m_field.text.empty()) {
        return make_placeholder();
    } else {
        return m_font.render_blended(m_field.text, consts::input_color);
    }
}

hal::surface console::make_placeholder() {
    return m_font.render_blended(generate_placeholder(), consts::placeholder_color);
}

std::string_view console::generate_placeholder() {
    if (m_placeholderIndex == std::size(console_placeholders)) { // Need to refresh.
        HAL_PRINT("<Console> Shuffling placeholder indices...");
        std::shuffle(std::begin(m_placeholderOrder), std::end(m_placeholderOrder), std::mt19937_64 { std::random_device {}() });
        m_placeholderIndex = 0;
    }

    return console_placeholders[m_placeholderOrder[m_placeholderIndex++]];
}

void console::set_cursor() {
    m_outline.pos.x = m_texBegin + (m_field.cursor % m_lineChars) * m_outline.size.x;
    m_outline.pos.y = consts::text_offset.y + m_outline.size.y * static_cast<std::uint8_t>(m_field.cursor / m_lineChars);

    m_cursorTime = consts::cursor_blink_time / 2.0;
    m_cursorVis  = true;
}
